import json
import urllib.request
import pymongo
	
#station	
#name	"Jayanagar 5th Block, Bengaluru, India"
#geo	
#0	12.920984
#1	77.584908

#station	
#name	"City Railway Station, Bangalore, India"
#geo	
#0	12.9773472
#1	77.570697222222

#station	
#name	"Hebbal, Bengaluru, India"
#geo	
#0	13.029152
#1	77.585901

#station	
#name	"BTM, Bangalore, India"
#geo	
#0	12.91281111
#1	77.60921944

#station	
#name	"Bapuji Nagar, Bengaluru, India"
#geo	
#0	12.951913
#1	77.539784

#station	
#name	"Peenya, Bangalore, India"
#geo	
#0	13.0339
#1	77.51321111

#station	
#name	"Silk Board, Bengaluru, India"
#geo	
#0	12.917348
#1	77.622813

#station	
#name	"SaneguravaHalli, Bangalore, India"
#geo	
#0	12.9916694
#1	77.545830555556

#station	
#name	"Hombegowda Nagar, Bengaluru, India"
#geo	
#0	12.938539
#1	77.5901

myclient = pymongo.MongoClient("mongodb://localhost:27017/")
mydb = myclient["reference"]

url = "https://api.waqi.info/feed/Jayanagar%205th%20Block,%20Bengaluru,%20India/?token=ed91aa58cdb0587d935eebdb3038fa9b0ac8a3cb"
req = urllib.request.Request(url)
response = urllib.request.urlopen(req)
data = response.read()
values = json.loads(data)
if values["status"] == "ok":
    result = mydb.mycol.insert_one(values)
    print('Inserted post id %s ' % result.inserted_id)

url = "https://api.waqi.info/feed/Hebbal,%20Bengaluru,%20India/?token=ed91aa58cdb0587d935eebdb3038fa9b0ac8a3cb"
req = urllib.request.Request(url)
response = urllib.request.urlopen(req)
data = response.read()
values = json.loads(data)
if values["status"] == "ok":
    result = mydb.mycol.insert_one(values)
    print('Inserted post id %s ' % result.inserted_id)
