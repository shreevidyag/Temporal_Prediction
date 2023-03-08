import pymongo

myclient = pymongo.MongoClient("mongodb://localhost:27017/")
mydb = myclient["reference"]
mycol = mydb["mycol"]

myquery = {}

mydoc = mycol.find({"data.idx":11428})

for x in mydoc:
  print([x["data"]["time"]["s"], x["data"]["iaqi"]["pm25"]["v"], x["data"]["iaqi"]["co"]["v"], x["data"]["iaqi"]["no2"]["v"], x["data"]["iaqi"]["so2"]["v"], x["data"]["iaqi"]["o3"]["v"], x["data"]["iaqi"]["t"]["v"], x["data"]["iaqi"]["w"]["v"]])
