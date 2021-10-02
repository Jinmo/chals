const Koa = require('koa'),
    Router = require('@koa/router'),
    session = require('koa-session'),
    bodyParser = require('koa-bodyparser'),
    send = require('koa-send'),
    serve = require('koa-static'),
    crypto = require('crypto'),
    fs = require('fs'),
    settings = require('./settings'),
    child_process = require('child_process'),
    md5 = require('./md5');

const app = new Koa()
const router = new Router()
const DB = new Map()
const usedPows = new Set()

const admin_pw = crypto.randomBytes(20).toString('hex')
fs.writeFileSync('/tmp/password', admin_pw)
DB.set('admin', {
    pw: admin_pw,
    memos: new Map([
        [crypto.randomBytes(20).toString('hex'), fs.readFileSync('/flag', 'utf-8')]
    ])
})

const auth = async (ctx, next) => {
    if (ctx.session.id)
        await next()
    else ctx.redirect('/')
}

const csp = async (ctx, next) => {
    ctx.set('Content-Security-Policy', `default-src 'none'; connect-src 'self'; script-src 'self'; style-src 'self'`)
    await next(ctx)
}

router
    .get('/', async ctx => {
        if (ctx.session.id)
            ctx.redirect('/memo/')
        else
            await send(ctx, 'templates/index.html')
    })
    .post('/memo/login', async ctx => {
        const { id, pw } = ctx.request.body
        if (!id || !pw) {
            return ctx.body = 'id, pw is required'
        }

        if (!DB.has(id)) {
            DB.set(id, { pw, memos: new Map() })
            ctx.session.id = id
            return ctx.redirect('/')
        } else {
            const { pw: userpw } = DB.get(id)
            if (userpw === pw) {
                ctx.session.id = id
                return ctx.redirect('/')
            } else {
                return ctx.body = 'password do not match'
            }
        }
    })
    .get('/memo/', auth, async ctx => {
        await send(ctx, 'templates/memo.html')
    })
    .post('/memo/', auth, async ctx => {
        const db = DB.get(ctx.session.id)
        const { title, content } = ctx.request.body

        if(ctx.session.id !== 'admin')
            db.memos.set(title, content)

        ctx.redirect('/')
    })
    .get('/memo/:id/:name', async ctx => {
        const { id, name } = ctx.params
        const db = DB.get(id)

        if (!db)
            return ctx.body = 'not found'

        ctx.body = db.memos.get(name) || 'not found'
        ctx.type = 'text/plain; charset=utf-8'
    })
    .get('/search', auth, async ctx => {})
    .get('/memo/session', auth, async ctx => {
        const session = {}
        const { id } = ctx.session
        const { memos } = DB.get(id)
        session.id = id
        session.memos = [...memos.keys()]
        session.host = settings.HOST
        ctx.body = session
    })
    .get('/memo/logout', auth, async ctx => {
        delete ctx.session.id;
        return ctx.redirect('/')
    })
    .post('/report', auth, async ctx => {
        const {url, pow} = ctx.request.body
        ctx.type = 'text/plain'

        if(!ctx.session.pow || usedPows.has(ctx.session.pow) || !md5(ctx.session.pow + pow).startsWith('00000'))
            return ctx.body = 'pow error!'

        ctx.session.pow = null
        usedPows.add(ctx.session.pow)

        if(url && typeof url === 'string' && url.startsWith(settings.HOST)) {
            child_process.execFile('node', [__dirname + '/bot.js', url])
            ctx.body = 'bot.js launched!'
        } else {
            ctx.body = 'url not valid: ' + url
        }
    })
    .get('/pow', auth, async ctx => {
        ctx.body = ctx.session.pow = crypto.randomBytes(8).toString('hex')
    })

app.keys = [crypto.randomBytes(20).toString('hex')]

app
    .use(session(app))
    .use(csp)
    .use(bodyParser())
    .use(router.routes())
    .use(router.allowedMethods())
    .use(serve('.'))

app.listen(80)