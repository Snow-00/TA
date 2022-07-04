from datetime import datetime, date
import requests
import pandas as pd


old_entry = 0
while(True):
    # get only the newest data from thingspeak
    url = requests.get("https://api.thingspeak.com/channels/1728108/feeds.json?result=1&timezone=Asia%2FJakarta")
    upd_time = pd.to_datetime(url.json()['feeds'][-1]['created_at'])
    upd_time = datetime.combine(date.min, datetime.time(upd_time))
    cur_time = datetime.combine(date.min, datetime.now().time())
    duration = cur_time - upd_time
    entry = url.json()['feeds'][-1]['entry_id']

    # check the duration of current time and time stated in thingspeak > 60
    if duration.total_seconds() >= 60:
        print(duration)
        break

    # check whether there's a new entry or not
    if (entry-old_entry) < 1:
        continue

    # extract the data from field 1 and 2
    msg1=url.json()['feeds'][-1]['field1']
    msg2=url.json()['feeds'][-1]['field2']
    print("The Message sent was: "+str(msg1)+" and "+str(msg2))
    old_entry = entry
    print(old_entry)