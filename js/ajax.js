// fetch the contents from the URL "url".  Once successful, apply the function
// "cont()" to the contents. If unsuccessful, apply the function err(msg).
function read_from_url(url, cont, err, mimetype) {

    var undefined;
    let req;
    if (mimetype == undefined) mimetype = 'text/xml';
    if (window.XMLHttpRequest) { // Mozilla, Safari, ...
        req = new XMLHttpRequest();
        if (req.overrideMimeType)
            req.overrideMimeType(mimetype);
    } else if (window.ActiveXObject) { // IE
        req = new ActiveXObject("Microsoft.XMLHTTP");
    }
    req.onreadystatechange = function() {
        if (req.readyState == 4) {
            if (req.status == 200) {
                cont(req.responseText)
            } else {
                err('Could not read from ' + url + ': Error ' + req.status);
            }
        }
    }
    // A URL that names a path from the root of the site is already where it
    // says it is; only one relative to the page it appears on needs the
    // page's own directory put in front of it.
    if (!url.match(/^(https?:|\/)/)) {
        var prefix = location.href;
        url = prefix.replace(/\/[^\/]*$/, '/') + url;
    }
    req.open("GET", url, true);
    req.send(null);
}

function post_to_url(url, params, cont, err, mimetype) {
    var undefined;
    let req;
    if (mimetype == undefined) mimetype = 'text/xml';
    if (window.XMLHttpRequest) { // Mozilla, Safari, ...
        req = new XMLHttpRequest();
        if (req.overrideMimeType)
            req.overrideMimeType(mimetype);
    } else if (window.ActiveXObject) { // IE
        req = new ActiveXObject("Microsoft.XMLHTTP");
    }
    req.onreadystatechange = function() {
        if (req.readyState == 4) {
            if (req.status == 200) {
                cont(req.responseText)
            } else {
                err('Could not read from ' + url + ': Error ' + req.status);
            }
        }
    }
    // A URL that names a path from the root of the site is already where it
    // says it is; only one relative to the page it appears on needs the
    // page's own directory put in front of it.
    if (!url.match(/^(https?:|\/)/)) {
        var prefix = location.href;
        url = prefix.replace(/\/[^\/]*$/, '/') + url;
    }

    var data = new FormData();
    for (var prop in params) {
	data.append(prop, params[prop]);
    }

    req.open("POST", url, true);
    req.send(data);
}

// fetch the contents from the URL "url" into the DOM node with id attribute
// "id".  Once successful, apply the optionally provided function "cont()".
// If the fetch fails, the failure is reported in the node itself -- unless
// "onerr()" is provided, which is then called instead, for a box whose
// contents are optional and which already says something sensible when
// there is nothing to put in it.
function fetch_content(id, url, cont, onerr) {
    const node = document.getElementById(id);
    read_from_url(url,
        function(responseText) {
            node.innerHTML = responseText;
            var undefined;
            if (cont != undefined) cont();
        },
        function(errmsg) {
            var undefined;
            if (onerr != undefined) { onerr(); return }
            node.innerHTML = 'Could not read from ' + url + ': ' + errmsg
        });
}
