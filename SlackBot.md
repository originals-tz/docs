# SlackBot

官方教程 https://github.com/slackapi/python-slack-sdk

App需要几个token，直接硬编码就好

可以开一个线程监听消息，一个线程处理业务

监听消息需要为机器人设置公开的url，可以用ngrok做反向代理，将自身的机器人暴露出去

设置url是需要注意，例如ngrok分配的url是 `https://mybot.ap.ngrok.io`,那么验证的时候需要填写`https://mybot.ap.ngrok.io/slack/events`

```python
import logging
import time
import re
import requests
import threading
from slack_bolt import App

#需要填token
app = App(token='xxx', signing_secret='xxx', verification_token='xxx')


def check_update(app):
    app.client.chat_postMessage(channel="comics", text="update")

#收到消息的回调
@app.event("message")
def message(event, client):
	msg = event.get("text")
	print(msg)
    app.client.chat_postMessage(channel="comics", text="get msg")

class SlackThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        logger = logging.getLogger()
        logger.setLevel(logging.DEBUG)
        logger.addHandler(logging.StreamHandler())
        app.start(5678) # 阻塞监听

class CheckThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        while True:
            check_update(app)
			time.sleep(3) #3s发一次消息

init_db()
tcheck = CheckThread()
tcheck.start()

tslack = SlackThread()
tslack.run()
```