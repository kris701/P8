import datetime

def ConvertSecToTimeFormat(sec : int) -> str:
    return str(datetime.timedelta(seconds=sec))