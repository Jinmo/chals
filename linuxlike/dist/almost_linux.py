#!/usr/bin/env python
import functools
import os
import uuid
from hashlib import sha256

from flask import Flask, session, render_template, g, request, \
    redirect, url_for, send_file

KERNEL_INFO = os.popen('uname -srv').read()
TEMPDIR = os.popen('wslpath $(cmd.exe /c echo %TEMP%)').read().strip()

app = Flask(__name__)
app.secret_key = os.urandom(12).encode('hex')


@app.before_request
def ensure_session():
    session['sessid'] = session.get('sessid') or str(uuid.uuid4())

    g.path = os.path.join(TEMPDIR, session['sessid'])
    if not os.path.isdir(g.path):
        os.makedirs(g.path, 0777)

    session['files'] = \
        {sha256(x).hexdigest(): x for x in os.listdir(g.path)}


@app.route('/')
def index():
    return render_template('index.html', uname=KERNEL_INFO, files=session['files'])


def secure_filename(name):
    name = reduce(lambda x, y: x.replace(y, u'-'), u'\\/:*?"<>|', name)
    return name or '_'


@app.route('/post', methods=['POST'])
def post():
    for f in request.files.values():
        name = secure_filename(f.filename)
        path = os.path.join(g.path, name)
        f.save(path)

    return redirect(url_for('index'))


@app.route('/read/<uuid>')
def read(uuid):
    f = session['files'].get(uuid)
    return send_file(os.path.join(g.path, f), as_attachment=True, attachment_filename=f)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=10000, debug=False, threaded=True)
