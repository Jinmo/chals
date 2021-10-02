const $ = document.querySelector.bind(document)
const loading = value => $('#url').disabled = $('#report').disabled = value;

let host;

onload = async () => {
    const res = await fetch('./session')
    const json = await res.json()

    if (json) {
        for (const key of Object.keys(json)) {
            for (const node of document.querySelectorAll('.-' + key)) {
                node.textContent = json[key]
            }
        }

        const dest = document.querySelector('.list')
        for (const item of json.memos) {
            const link = document.createElement('a')
            link.href = `/memo/${json.id}/${encodeURIComponent(item)}`
            link.textContent = item
            dest.appendChild(link)
        }

        host = json.host

        $('#url').placeholder=host + '...';
        $('#report').onclick = event => {
            (async (url) => {
                if(!url.startsWith(host)) {
                    alert('url must start with "' + host + '"!')
                    return
                }

                $('#report').textContent = 'Calculating PoW...'
                loading(true)
                const pow = await (await fetch('/pow')).text()
                const worker = new Worker('../pow_worker.js')

                worker.onmessage = event => {
                    loading(false)

                    $('#url').value = url
                    $('#pow').value = event.data
                    $('#report-form').submit()
                }
                worker.postMessage(pow)
            })($('#url').value)

            return false
        }
    }
}