#ifndef CSS_H
#define CSS_H

static const char APP_CSS[] PROGMEM = R"rawliteral(
:root{
  --bg:#0e1118; --card:#171a22; --muted:#9aa3b2; --text:#e9edf2; --ring:#2a2f3d;
  --accent:#3b82f6; --ok:#10b981; --danger:#ef4444; --btn:#272b39;
  --radius:14px; --shadow:0 10px 30px rgba(0,0,0,.35);
}
*{box-sizing:border-box}
html,body{height:100%}
body{margin:0; font-family:system-ui,-apple-system,Segoe UI,Roboto,Arial,sans-serif; background:var(--bg); color:var(--text);}
.wrap{max-width:1000px; margin:0 auto; padding:16px}
.card{background:linear-gradient(180deg,rgba(255,255,255,.02),rgba(255,255,255,0)),var(--card); border:1px solid var(--ring); border-radius:var(--radius); box-shadow:var(--shadow); overflow:hidden;}
.header{position:sticky; top:0; z-index:10; display:flex; align-items:center; justify-content:space-between; gap:12px; padding:14px 16px; border-bottom:1px solid var(--ring); background:rgba(23,26,34,.9); backdrop-filter:blur(6px) saturate(130%);}
.title{margin:0; font-size:18px; color:#cfd6e4; letter-spacing:.3px}
.stat{display:flex; align-items:center; gap:10px;}
#statusDot{width:10px; height:10px; border-radius:50%; background:#6b7280; border:1px solid #111;}
.statusText{font-size:12px; color:var(--muted)}
#timeDisplay{font-variant-numeric:tabular-nums; font-weight:800; font-size:44px; letter-spacing:1px; padding:6px 12px; border-radius:10px; background:#0f1220; border:1px solid var(--ring);}
.grid{display:grid; gap:14px; padding:16px; grid-template-columns:1fr;}
.field{display:flex; flex-direction:column; gap:6px}
.field label{font-size:12px; color:var(--muted); letter-spacing:.3px}
input[type=text]{width:100%; padding:12px 14px; font-size:18px; color:var(--text); background:#0f141c; border:1px solid var(--ring); border-radius:12px; outline:none; transition:border-color .15s ease;}
input[type=text]:focus{border-color:var(--accent)}
.chips{display:flex; flex-wrap:wrap; gap:8px}
.chip{font-size:14px; padding:8px 12px; border-radius:999px; cursor:pointer; user-select:none; background:#121722; border:1px solid var(--ring); color:#cfd6e4;}
.chip.active{border-color:var(--accent); color:#eaf2ff}
.controls{display:flex; flex-wrap:wrap; gap:10px; align-items:center; justify-content:center; padding:16px; border-top:1px solid var(--ring);}
button,.btn{appearance:none; border:none; cursor:pointer; text-decoration:none; padding:14px 18px; border-radius:12px; font-size:18px; font-weight:700; color:#fff; background:var(--btn);}
.start{background:#10b981}
.stop{background:#e11d48}
.ghost{background:#2a2f3d; color:#dbe5f4}
.btn{display:inline-block}
.results{border-top:1px solid var(--ring); padding:16px; background:#0f1218}
#log{margin:0}
#log h3{margin:0 0 10px 0; font-size:16px; color:#cfd6e4}
#results{margin-top:8px; width:100%; max-height:320px; overflow:auto; padding:12px; border:1px solid var(--ring); border-radius:10px; background:#0b0f15; font-family:ui-monospace,Menlo,Consolas,monospace; font-size:14px; line-height:1.5; white-space:pre-wrap;}
.toolbar{display:flex; gap:10px; flex-wrap:wrap; justify-content:center; padding:0 16px 16px 16px}
@media (min-width:720px){ .grid{grid-template-columns:1.2fr .9fr .9fr} #timeDisplay{font-size:52px}}
@media (prefers-reduced-motion: reduce){ *{transition:none !important} }

/* Light-mode: selected chip turns orange */
.light-mode #strokeChips .chip.active,
.light-mode #distanceChips .chip.active{
  background:#ff9800 !important; color:#ffffff !important; border-color:#ff9800 !important;
}
)rawliteral";
#endif