const puppeteer = require('puppeteer-core'),
    fs = require('fs'),
    { HOST, TIMEOUT } = require('./settings')

const read = path => fs.readFileSync('/tmp/' + path, 'utf-8')
let browser;

const execute = async (url) => {
    browser = await puppeteer.launch({
        executablePath: '/usr/bin/google-chrome',
        args: [
            '--no-sandbox',
            '--disable-setuid-sandbox',
            '--disable-dev-shm-usage'
        ]
    })
    const page = await browser.newPage()

    await page.goto(HOST)
    console.info('login...')

    await page.type('#id', 'admin')
    await page.type('#pw', read('password'))
    await Promise.all([
        page.waitForNavigation(),
        page.click('button')
    ])

    console.info('login completed')

    await page.goto(url, { timeout: TIMEOUT * 1000 })
    console.log('done')
}

if (process.argv.length !== 3 || !process.argv[2].startsWith(HOST)) {
    console.log('invalid url: ' + process.argv[2])
} else {
    execute(process.argv[2])
    setTimeout(() => {
        browser.close()
        process.exit(0)
    }, TIMEOUT * 1000)
}