FILENAME = "thingspeak_feed_tama.csv"
HEADER_FILE = "created_at,entry_id,field1,field2,field3,field4,field5,field6,field7,field8,elevation\n"
date = 23
hour = 6
min = 0
sec = 0

def create_time(date, hour, min, sec):
    sec_temp = sec + 1200
    min_temp = sec_temp // 60
    sec = sec_temp % 60

    min = min + min_temp
    hour_temp = min // 60
    min = min % 60

    hour = hour + hour_temp
    date_temp = hour // 24
    hour = hour % 24
    date = date + date_temp
    
    return date, hour, min, sec

with open(FILENAME, "w") as file:
    file.write(HEADER_FILE)
    i = 1
    while date <= 24:
        date, hour, min, sec = create_time(date, hour, min, sec)
        text = f"2022-07-{date}T{hour:02d}:{min:02d}:{sec:02d}+07:00,{i}\n"
        file.write(text)
        i += 1