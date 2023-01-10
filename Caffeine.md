## URL
[Caffeine高性能设计剖析 | Alben's home (albenw.github.io)](https://albenw.github.io/posts/a4ae1aa2/)

[TinyLFU 分析 (qin.news)](https://www.qin.news/tinylfu/)

[现代的缓存设计方案：Window-TinyLFU-爱代码爱编程 (icode.best)](https://icode.best/i/38783046086641)

[HONGYU-LEE/Window-TinyLFU-Cache: Window-TinyLFU Cache implemented in C++ (github.com)](https://github.com/HONGYU-LEE/Window-TinyLFU-Cache)

[Design zh CN · ben-manes/caffeine Wiki (github.com)](https://github.com/ben-manes/caffeine/wiki/Design-zh-CN)

[Caffeine基础源码解析_Ftsom的博客-CSDN博客_caffeine源码分析](https://blog.csdn.net/l_dongyang/article/details/123294062)

[Caffeine cache 学习01_郭传志的技术博客_51CTO博客](https://blog.51cto.com/u_6478076/5204120)
## 思路
![[1.png]]
如图所示，所有的数据都被存放到绿色的部分，也就是ConcurrentHashMap
这个ConcurrentHashMap中的数据在逻辑上被分为三块(基于W-TinyLFU算法)，由下面三个不同的队列(LRU)进行管理

```java
// windows区
protected final AccessOrderDeque<Node<K, V>> accessOrderEdenDeque() {  
  return accessOrderEdenDeque;  
}  

// probation区
protected final AccessOrderDeque<Node<K, V>> accessOrderProbationDeque() {  
  return accessOrderProbationDeque;  
}  

//protected区
protected final AccessOrderDeque<Node<K, V>> accessOrderProtectedDeque() {  
  return accessOrderProtectedDeque;
}
```

文档对此的描述是

>accessOrderDeque 顺序访问队列，访问顺序被定义为缓存中的元素被创建，更新或是被访问的顺序。最近最少被使用的元素将会在队首，而最近被使用的元素将会在队列末尾。这为基于容量的驱逐策略(`maximumSize`)和基于空闲时间的过期策略(`expireAfterAccess`)的实现提供了帮助。使用这个队列的挑战在于每次访问都将会对队列当中的节点发生改变，这无法有效的通过并发操作实现。

## Maintenancbe
caffeine 采用了异步+同步的方式去维护这些队列

维护的具体操作如下

```java
@GuardedBy("evictionLock")  
void maintenancbe(@Nullable Runnable task) {  
  lazySetDrainStatus(PROCESSING_TO_IDLE);  
  
  try {  
   
    drainReadBuffer();  


    drainWriteBuffer();  
    if (task != null) {  
      task.run();  
    }  
  
    drainKeyReferences();  
    drainValueReferences();  
  
    expireEntries();  
    evictEntries();  
  } finally {  
    if ((drainStatus() != PROCESSING_TO_IDLE) || !casDrainStatus(PROCESSING_TO_IDLE, IDLE)) {  
      lazySetDrainStatus(REQUIRED);  
    }  
  }  
}
```

### 操作
#### drainReadBuffer
```
void drainReadBuffer() {  
  if (!skipReadBuffer()) {  
    readBuffer.drainTo(accessPolicy);  
  }  
}
```
使用accessPolicy(lambda)来清空readbuffer(RingBuffer) 主要是调整节点在LRU的位置
`accessPolicy = (evicts() || expiresAfterAccess()) ? this::onAccess : e -> {};`


### 触发

对于维护操作，Caffeine通过状态**四个状态和原子操作**来尽可能提高效率

```java
    /** A drain is not taking place. */
    static final int IDLE = 0; //空闲，可以执行
    /** A drain is required due to a pending write modification. */
    static final int REQUIRED = 1; //需要执行任务，该状态下次会执行
    /** A drain is in progress and will transition to idle. */
    static final int PROCESSING_TO_IDLE = 2; //进行中，执行完毕后，进入到空闲状态
    /** A drain is in progress and will transition to required. */
    static final int PROCESSING_TO_REQUIRED = 3; //执行中，执行完毕后，到需要执行任务状态
```

维护操作的触发时机

#### 读操作
- afterRead, 访问对应节点时，会将所访问的节点放入ReadBuffer(允许丢失)

```java
void afterRead(Node<K, V> node, long now, boolean recordHit) {  
  if (recordHit) {  
    statsCounter().recordHits(1);  
  }  
  
  boolean delayable = skipReadBuffer() || (readBuffer.offer(node) != Buffer.FULL);  
  if (shouldDrainBuffers(delayable)) {  
    scheduleDrainBuffers();  
  }  
  refreshIfNeeded(node, now);  
}
```

调度
```java
void afterRead(Node<K, V> node, long now, boolean recordHit) {  
  if (recordHit) {  
    statsCounter().recordHits(1);  
  }  
  
  boolean delayable = skipReadBuffer() || (readBuffer.offer(node) != Buffer.FULL);  
  if (shouldDrainBuffers(delayable)) {  
    scheduleDrainBuffers();  
  }  
  refreshIfNeeded(node, now);  
}

void scheduleDrainBuffers() {  
  if (drainStatus() >= PROCESSING_TO_IDLE) {  
    return;  
  }  
  if (evictionLock.tryLock()) {   //尝试上锁
    try {  
      int drainStatus = drainStatus();  
      //是否已经有任务在执行
      if (drainStatus >= PROCESSING_TO_IDLE) {  
        return;  
      }  
      
      lazySetDrainStatus(PROCESSING_TO_IDLE);  
      // 执行任务
      executor().execute(drainBuffersTask);  
    } catch (Throwable t) {  
      logger.log(Level.WARNING, "Exception thrown when submitting maintenance task", t);  
      // 同步操作
      maintenance(/* ignored */ null);  
    } finally {  
      evictionLock.unlock();  
    }  
  }  
}

```

#### 写操作
- afterWrite，修改对应的节点，生成对应的Task(AddTask/RemoveTask/UpdateTask), 放入writeBuffer，如果多次存入失败，那么直接在本线程执行维护操作，write操作记录不允许丢失

写操作的调度

```java
void scheduleAfterWrite() {  
  for (;;) {  
    switch (drainStatus()) {  
      case IDLE:  
        casDrainStatus(IDLE, REQUIRED);  
        scheduleDrainBuffers();  
        return;  
      case REQUIRED:  
        scheduleDrainBuffers();  
        return;  
      case PROCESSING_TO_IDLE:
        if (casDrainStatus(PROCESSING_TO_IDLE, PROCESSING_TO_REQUIRED)) {  
          return;  
        }  
        continue;
        // 如果cas失败，那么继续该流程，将循环写在外部，应该是这个状态可能会被其他线程修改成功，这时候，下一个循环就需要做不同的处理
      case PROCESSING_TO_REQUIRED:  
        return;  
      default:  
        throw new IllegalStateException();  
    }  
  }  
}
```


