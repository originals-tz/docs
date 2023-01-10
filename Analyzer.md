[Elasticsearch中什么是 tokenizer、analyzer、filter ? - 腾讯云开发者社区-腾讯云 (tencent.com)](https://cloud.tencent.com/developer/article/1706529)

Analyzer = Tokenizer + Filter
![[2.png]]
对于数据的数据，先使用Tokenizer分词，再使用Filter过滤数据

### 分析器(Analyzer)
无法修改现有分析器
https://dba.stackexchange.com/questions/194659/change-analyzer-for-an-elasticsearch-index

测试时可以新增一个简单的索引
```json
PUT test
{
	"mappings": {
		"properties": {
			"context": {
				"type": "text",
				"analyzer": "html_text_analyzer"
			}
		}
	},
	"settings": {
		"index": {
			"analysis": {
				"analyzer": { #创建自定义分析器
					"html_text_analyzer": {
						"char_filter": [
							"html_strip"
						],
						"tokenizer": "standard"
					}
				}
			}
		}
	}
}
```
测试
```json
PUT test/_analyze 使用对应index中的分词器
{
	"analyzer": "html_text_analyzer",
	"text": "需要测试的数据"
}
```
