import functools
import os
import uuid
from hashlib import sha256

from flask import Flask, session, render_template, g, request, \
    redirect, url_for, send_file

app = Flask(__name__)
app.secret_key = os.urandom(12).encode('hex')
uname = os.popen('uname -srv').read()
temp_path = os.popen('wslpath $(cmd.exe /c echo %TEMP%)').read().strip()


def get_session_path(sessid):
    x = os.path.join(temp_path, sessid)
    if not os.path.isdir(x):
        os.makedirs(x, 0777)
    return x


def session_required(f):
    @functools.wraps(f)
    def handler(*args, **kwargs):
        if 'sessid' not in session or any(x not in '0123456789abcdef-' for x in session['sessid']):
            session['sessid'] = str(uuid.uuid4())

        g.path = get_session_path(session['sessid'])
        session['files'] = {sha256(x).hexdigest(): x for x in os.listdir(g.path)}

        return f(*args, **kwargs)

    return handler


# some file related stuffs

def secure_filename(name):
    r = reduce(lambda x, y: x.replace(y, u'-'), u'\\/:*?"<>|.-', name)
    return r or '_'


@app.route('/')
@session_required
def index():
    return render_template('index.html', uname=uname, files=session['files'])


@app.route('/post', methods=['POST'])
@session_required
def post():
    for f in request.files.values():
        name = secure_filename(f.filename)
        path = os.path.join(g.path, name)
        f.save(path)

    return redirect(url_for('index'))


@app.route('/read')
@session_required
def read():
    _uuid = request.args['uuid']
    f = session['files'].get(_uuid)
    if not f:
        return abort(404)
    else:
        return send_file(os.path.join(g.path, f), as_attachment=True, attachment_filename=f)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=10000, debug=False, threaded=True)
