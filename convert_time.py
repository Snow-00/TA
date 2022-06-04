import requests
import pandas as pd
import datetime as dt

msg = requests.get("https://api.thingspeak.com/channels/1728108/feeds.json?timezone=Asia%2FJakarta")
time = pd.to_datetime(msg.json()['feeds'][-1]['created_at'])
print(time)
print(type(time))

new_time = dt.datetime.time(time)
print(new_time)

cur_time = dt.datetime.now().time()
print(cur_time)

new_format = dt.datetime.combine(dt.date.min, cur_time)
print(type(new_format))
duration = dt.datetime.combine(dt.date.min, cur_time) - dt.datetime.combine(dt.date.min, new_time)
print(duration.total_seconds())
print(type(duration.total_seconds()))