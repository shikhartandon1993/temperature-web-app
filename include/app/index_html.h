#ifndef INDEX_HTML_H
#define INDEX_HTML_H

static const char index_html[] = 
"<!DOCTYPE html>"
"<html>"
"<head><title>Temperature Monitor</title></head>"
"<body>"
"<h1>Current Temperature</h1>"
"<div id='temp'>Loading...</div>"
"<script>"
"async function updateTemp(){"
"     let r=await fetch('/temp');"
"     let t=await r.text();"
"     document.getElementById('temp').innerText=t;"
"}"
"setInterval(updateTemp,1000);"
"updateTemp();"
"</script>"
"</body>"
"</html>";

static const size_t index_html_len = sizeof(index_html) - 1;

#endif
