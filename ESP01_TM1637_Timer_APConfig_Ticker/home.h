const char home_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CONTROLER Config</title>
</head>

<body>
    <style>
        body {
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            background: linear-gradient(to top, rgb(0, 162, 255), aqua);
        }

        h2,
        h3 {
            text-align: center;
            font-weight: bold;
            margin: 0;
        }

        h6 {
            text-align: center;
            font-size: 10px;
            margin: 0;
            text-decoration: none;
        }

        h5 {
            text-align: center;
            font-size: 10px;
            margin: 0;
            color: rgb(0, 195, 255);
            font-weight: 100;
        }


        a[id=tmblmulai],
        [id=tmblberhenti],
        [id=tmblconfig] {
            border-radius: 10px;
            font-family: 'Franklin Gothic Medium', 'Arial Narrow', Arial, sans-serif;
            text-decoration: none;
            text-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
            padding: 10px;
            color: rgb(232, 232, 232);
            background-color: #45A049;
        }

        a[id=tmblmulai] {
            /* background-color: #45A049; */
            background: linear-gradient(0deg, rgb(0, 90, 7) 0%, rgb(26, 255, 0) 100%);
        }

        a[id=tmblberhenti] {
            /* background-color: #f90000; */
            /* background: linear-gradient(0deg, rgba(162, 0, 0, 1) 0%, rgba(255, 0, 0, 1) 100%); */
            background: linear-gradient(0deg, rgb(64, 66, 0) 0%, rgb(255, 247, 0) 100%);
        }

        a[id=tmblconfig] {
            /* background-color: #1f1f1f; */
            background: linear-gradient(0deg, rgb(77, 77, 77) 0%, rgb(208, 208, 208) 100%);
        }

        a:hover {
            box-shadow: rgba(0, 0, 0, 0.753) 5px 5px 20px;
        }

        #tombolGroup {
            display: flex;
            justify-content: space-around;
        }

        #tombolSET {
            display: flex;
            justify-content: center;
        }

        .container {
            border-radius: 10px;
            background-color: #ffffff;
            padding: 5px;
            margin: 10%;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
        }

        .content {
            background-color: #363636;
            color: #ffffff;
            box-shadow: rgba(48, 39, 71, 0.753) 3px 3px 5px;
            padding: 20px;
            border-radius: 10px;
        }

        #theme {
            position: absolute;
            right: 0;
            top: 0;
            margin-top: 10%;
            background: linear-gradient(to top, rgb(0, 162, 255), aqua);
            border: none;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 4px 8px;
            padding: 10px;
            border-radius: 10px 0px 0px 10px;
            border-left: #c7c6c6 solid;
            border-bottom: #9d9d9d solid;
            border-top: #f1f1f1 solid;
        }

        #theme:hover {
            cursor: pointer;
        }

        .light-mode {
            background-color: #ffffff;
            color: #000000;
        }

        .light-mode-container {
            background-color: #787878;
            color: #ffffff;
        }

        #stringJam {
            font-size: 42px;
            font-weight: bolder;
            text-align: center;
            margin: 10px;
        }

        #tmblkembali {
            background: linear-gradient(0deg, rgb(0, 0, 0) 0%, rgb(49, 49, 49) 100%);
            border: none;
            color: #f1f1f1;
            padding: 5px;
            font-size: 16px;
            border-radius: 5px;
            margin-top: 10px;
            text-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
            text-align: center;
            text-decoration: none;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            display: block;
            margin-left: auto;
            margin-right: auto;
        }
    </style>

    <div class="container" id="container">
        <div class="content" id="content">
            <div id="beranda">
                <h2>CONTROL</h2>
                <h3>Status Device</h3>
                <hr>
                <div id="stringJam">%His%</div>

                <div id="tombolGroup">
                    <a id="tmblmulai" href="#" onclick="toggleStartStop(); return false;">%START%</a>
                    <a id="tmblberhenti" href="#" onclick="resetTimer(); return false;">RESET</a>

                </div>
                <hr>
                <div id="tombolSET">
                    <a id="tmblconfig" href="/config?setJam=%setjam%&setMenit=%setmenit%&setDetik=%setdetik%">Config</a>
                </div>
            </div>
            <hr>
            <a id="tmblkembali" href="%href%">Kembali</a>

            <button id="theme" onclick="toggleTheme()">Light</button>

        </div>
    </div>

    <script>
        var tmblmulai = document.getElementById('tmblmulai');
        var tmblberhenti = document.getElementById('tmblberhenti');

        // Mendapatkan parameter GET 'run' dari URL
        const urlParams = new URLSearchParams(window.location.search);
        const runParameter = urlParams.get('run');
        const userParam = urlParams.get('username');
        const passParam = urlParams.get('password');

        if (runParameter === "true") {
            tmblmulai.textContent = "STOP";
            tmblmulai.style = "background: linear-gradient(0deg, rgba(162, 0, 0, 1) 0%, rgba(255, 0, 0, 1) 100%);";
            tmblberhenti.disabled = false; //
        } else if (runParameter === "false") {
            tmblmulai.textContent = "START";
            tmblmulai.style = "background: linear-gradient(0deg, rgba(0, 130, 11, 1) 0%, rgba(21, 204, 0, 1) 100%);";
            tmblberhenti.disabled = true;
        }

        function updateStringJam() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("stringJam").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/getStringJam", true);
            xhttp.send();
        }

        setInterval(updateStringJam, 1000);  // Perbarui setiap detik

        function toggleTheme() {
            var content = document.getElementById('content');
            var container = document.getElementById('container');
            var listssid = document.getElementById('listssid');
            var theme = document.getElementById('theme');
            content.classList.toggle('light-mode');
            container.classList.toggle('light-mode-container');

            // Periksa apakah elemen memiliki kelas 'light-mode'
            var isLightMode = content.classList.contains('light-mode');

            // Setel teks tombol berdasarkan tema yang aktif
            theme.textContent = isLightMode ? "Dark" : "Light";
        }

        function toggleStartStop() {
            var tmblmulai = document.getElementById('tmblmulai');
            var tmblberhenti = document.getElementById('tmblberhenti');

            if (tmblmulai.textContent === "START") {
                updateUrl("?run=true&username=" + userParam + "&password=" + passParam);
            } else {
                updateUrl("?run=false&username=" + userParam + "&password=" + passParam);
            }
        }

        function updateUrl(query) {
            var newurl = window.location.protocol + "//" + window.location.host + window.location.pathname + query;
            window.history.pushState({ path: newurl }, '', newurl);

            // Force a page reload
            window.location.href = newurl;
        }


        function resetTimer() {
            var tmblmulai = document.getElementById('tmblmulai');
            var tmblberhenti = document.getElementById('tmblberhenti');

            // Only perform the reset if the timer is in the "START" state
            if (tmblmulai.textContent === "START") {
                // Your resetTimer logic here
                updateUrl("?run=reset&username=" + userParam + "&password=" + passParam);
            } else {
                alert("Harap hentikan timer sebelum reset");
            }
        }
    </script>
</body>

</html>
)rawliteral";