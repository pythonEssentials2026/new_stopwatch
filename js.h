#ifndef JS_H
#define JS_H

// Serve this with:
// server.on("/app.js", [](){ server.send_P(200, "application/javascript", APP_JS); });

static const char APP_JS[] PROGMEM = R"rawliteral(
function updateTime(){
  fetch('/time')
    .then(function(res){ return res.text(); })
    .then(function(text){
      var el = document.getElementById('timeDisplay');
      if(el) el.textContent = text;
      setStatus(true);
    })
    .catch(function(){ setStatus(false); });
}
setInterval(updateTime, 120);

// Helpers for chips
function setActiveByValue(groupId, value){
  var g = document.getElementById(groupId);
  if(!g) return;
  var chips = g.getElementsByClassName('chip');
  var found = false;
  for(var i=0;i<chips.length;i++){
    if(chips[i].getAttribute('data-val') === value){ chips[i].classList.add('active'); found = true; }
    else{ chips[i].classList.remove('active'); }
  }
  if(!found && chips.length){ chips[0].classList.add('active'); }
}
function getActiveValue(groupId){
  var g = document.getElementById(groupId);
  if(!g) return "";
  var a = g.querySelector('.chip.active');
  return a ? a.getAttribute('data-val') : "";
}
function wireChipGroup(groupId, storageKey){
  var g = document.getElementById(groupId);
  if(!g) return;
  g.addEventListener('click', function(e){
    var t = e.target;
    if(t && t.classList.contains('chip')){
      var chips = g.getElementsByClassName('chip');
      for(var i=0;i<chips.length;i++) chips[i].classList.remove('active');
      t.classList.add('active');
      if(storageKey){ localStorage.setItem(storageKey, t.getAttribute('data-val')); }
    }
  });
  var stored = storageKey ? localStorage.getItem(storageKey) : "";
  if(stored){ setActiveByValue(groupId, stored); }
  else {
    var chips = g.getElementsByClassName('chip');
    if(chips.length) chips[0].classList.add('active');
  }
}
wireChipGroup('strokeChips', 'swim_stroke');
wireChipGroup('distanceChips', 'swim_distance');

// --- Theme toggle (light/dark) ---
(function () {
  var key = 'swim_theme';
  var btn = document.getElementById('themeToggle');
  if (!btn) return;

  // toggle a body class so CSS can style light-mode chip selection
  function setBodyClass(theme){
    if(theme === 'light'){ document.body.classList.add('light-mode'); }
    else{ document.body.classList.remove('light-mode'); }
  }

  function dynStyle() {
    var s = document.getElementById('dynTheme');
    if (!s) { s = document.createElement('style'); s.id = 'dynTheme'; document.head.appendChild(s); }
    return s;
  }

  function apply(theme) {
    var s = dynStyle();
    setBodyClass(theme);

    if (theme === 'light') {
      // Preschool palette
      document.documentElement.style.setProperty('--bg',   '#fff8e1');
      document.documentElement.style.setProperty('--card', '#ffffff');
      document.documentElement.style.setProperty('--text', '#333333');
      document.documentElement.style.setProperty('--ring', '#ffcc80');
      document.documentElement.style.setProperty('--btn',  '#29b6f6');

      // Buttons
      var starts = document.getElementsByClassName('start');
      for (var i=0;i<starts.length;i++) starts[i].style.background = '#66bb6a';
      var stops = document.getElementsByClassName('stop');
      for (var j=0;j<stops.length;j++) stops[j].style.background = '#ef5350';
      var ghosts = document.getElementsByClassName('ghost');
      for (var k=0;k<ghosts.length;k++) ghosts[k].style.background = '#ffb74d';

      // Timer: black bg + white digits
      var t = document.getElementById('timeDisplay');
      if (t) { t.style.background = '#000000'; t.style.color = '#ffffff'; }

      // Inputs (#name, #pool) and results: white text
      var els = document.querySelectorAll('input, #results');
      for (var m=0;m<els.length;m++) els[m].style.color = '#ffffff';

      // Orange backgrounds ONLY in light mode for inputs
      var ids = ['name','pool'];
      for (var n=0;n<ids.length;n++){
        var el = document.getElementById(ids[n]);
        if (el) el.style.background = '#ff9800';
      }

      // Placeholder + results color (chip color handled by CSS via .light-mode)
      s.textContent =
        'input::placeholder{color:#ffffff !important;opacity:0.8;}' +
        '#results{color:#ffffff !important;}';

      btn.textContent = 'Dark Theme';
      localStorage.setItem(key, 'light');
      return;
    }

    // DARK THEME (restore originals)
    document.documentElement.style.setProperty('--bg',   '#0e1118');
    document.documentElement.style.setProperty('--card', '#171a22');
    document.documentElement.style.setProperty('--text', '#e9edf2');
    document.documentElement.style.setProperty('--ring', '#2a2f3d');
    document.documentElement.style.setProperty('--btn',  '#272b39');

    var starts2 = document.getElementsByClassName('start');
    for (var i2=0;i2<starts2.length;i2++) starts2[i2].style.background = '#10b981';
    var stops2 = document.getElementsByClassName('stop');
    for (var j2=0;j2<stops2.length;j2++) stops2[j2].style.background = '#e11d48';
    var ghosts2 = document.getElementsByClassName('ghost');
    for (var k2=0;k2<ghosts2.length;k2++) ghosts2[k2].style.background = '#2a2f3d';

    var t2 = document.getElementById('timeDisplay');
    if (t2) { t2.style.background = '#0f1220'; t2.style.color = '#ffffff'; }

    // Clear inline overrides so base dark CSS applies
    var els2 = document.querySelectorAll('input, #results');
    for (var r=0;r<els2.length;r++){
      els2[r].style.color = "";
      els2[r].style.background = "";
    }

    // placeholders/results back to dark defaults
    dynStyle().textContent =
      'input::placeholder{color:#9aa3b2;opacity:0.8;}' +
      '#results{color:var(--text);}';

    btn.textContent = 'Light Theme';
    localStorage.setItem(key, 'dark');
  }

  // Initial apply (saved or default dark)
  apply(localStorage.getItem(key) || 'dark');

  // Toggle on click
  btn.addEventListener('click', function () {
    var next = (localStorage.getItem('swim_theme') === 'light') ? 'dark' : 'light';
    apply(next);
  });
})();

function setStatus(ok){
  var dot = document.getElementById('statusDot');
  var txt = document.getElementById('statusText');
  if(!dot || !txt) return;
  if(ok){ dot.style.background = '#10b981'; txt.textContent = 'Online'; }
  else { dot.style.background = '#ef4444'; txt.textContent = 'Offline'; }
}

function safeAction(path, btn){
  if(btn && btn.disabled) return;
  if(btn){ btn.disabled = true; setTimeout(function(){ btn.disabled = false; }, 300); }
  fetch(path).catch(function(){});
}

function submitTime(){
  var name = document.getElementById('name').value.trim();
  var pool = document.getElementById('pool').value.trim();
  var stroke = getActiveValue('strokeChips');
  var distance = getActiveValue('distanceChips');
  if(!name){ alert('Please enter a name'); return; }
  if(!pool){ pool = ""; }
  if(!stroke){ alert('Please choose a stroke'); return; }
  if(!distance){ alert('Please choose a distance'); return; }
  var params = new URLSearchParams({ name: name, pool: pool, stroke: stroke, distance: distance });
  fetch('/finish?' + params.toString())
    .then(function(){ location.reload(); })
    .catch(function(){ alert('Failed to submit'); });
}

function clearAll(){
  if(!confirm('Clear all results?')) return;
  fetch('/clear', { method:'POST' })
    .then(function(){
      var res = document.getElementById('results');
      if(res) res.innerHTML = "";
    });
}

function copyResults(){
  var res = document.getElementById('results');
  if(!res) return;
  var text = res.innerText || res.textContent || "";
  if(!text.trim()){ alert('No results to copy'); return; }
  navigator.clipboard.writeText(text).catch(function(){});
}

// Persist fields (name, pool)
(function(){
  ['name','pool'].forEach(function(id){
    var v = localStorage.getItem('swim_' + id);
    if(v !== null){
      var el = document.getElementById(id);
      if(el) el.value = v;
    }
    var el2 = document.getElementById(id);
    if(el2){
      el2.addEventListener('input', function(){ localStorage.setItem('swim_' + id, el2.value); });
      el2.addEventListener('change', function(){ localStorage.setItem('swim_' + id, el2.value); });
    }
  });
})();

document.addEventListener('keydown', function(e){
  var tag = (e.target && e.target.tagName) ? e.target.tagName : "";
  if(tag === 'INPUT' || e.altKey || e.ctrlKey || e.metaKey) return;
  var k = e.key.toLowerCase();
  if(k === 's'){ safeAction('/start', document.getElementById('btnStart')); }
  else if(k === 'x'){ safeAction('/stop', document.getElementById('btnStop')); }
  else if(k === 'r'){ safeAction('/reset'); }
  else if(k === 'f'){ submitTime(); }
});

updateTime();
)rawliteral";

#endif // JS_H
