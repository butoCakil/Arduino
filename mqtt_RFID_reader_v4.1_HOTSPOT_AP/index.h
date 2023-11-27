const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SiAPP Config</title>
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

        a {
            text-decoration: none;
            color: gray;
            text-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
        }

        input[type=text],
        select {
            width: 100%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }

        #password {
            width: 100%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 5px 0 0 5px;
            box-sizing: border-box;
        }

        input[type=submit] {
            width: 100%;
            background-color: #4CAF50;
            color: white;
            padding: 14px 20px;
            margin: 8px 0;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
        }

        input[type=submit]:hover {
            background-color: #45A049;
            box-shadow: rgba(96, 96, 96, 0.753) 2px 2px 5px;
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

        .inputPass {
            display: flex;
        }

        span {
            background-color: #ffffff;
            padding: 5px;
            color: aliceblue;
            border-radius: 0 5px 5px 0;
            margin: auto auto;
            height: 31px;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 5px;
        }

        span:hover {
            cursor: pointer;
            box-shadow: #a28787 1px 1px 3px;
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
        }

        .light-mode {
            background-color: #ffffff;
            color: #000000;
        }

        .light-mode-conrainer {
            background-color: #787878;
            color: #ffffff;
        }
    </style>


    <div class="container" id="container">
        <div class="content" id="content">
            <form action="/action_page" method="post">
                <h2>𝕊𝕚𝔸ℙℙ</h2>
                <hr>
                <h3>Konfigurasi Device</h3>
                <br><br>
                <label for="ssid">SSID</label>
                <input type="text" id="ssid" name="ssidNew" value="%SSID_NEW%" placeholder="SSID WiFi Anda" required>

                <label for="password">Password</label>
                <div class="inputPass">
                    <input type="password" id="password" name="passNew" value="%PASS_NEW%"
                        placeholder="Kosongkan jika tanpa password">
                    <span id="togglePassword" onclick="togglePassword()">
                        🙈
                    </span>
                </div>

                <label for="nodevice">Nomor Device</label>
                <input type="text" id="nodevice" name="nodevice" value="%NODEVICE%"
                    placeholder="Nomor yang telah terdaftar" required>

                <label for="host">HOST</label>
                <input type="text" id="host" name="host" value="%HOST%" placeholder="Alamat Host" required>

                <input type="submit" value="Simpan">

                <hr>
                <h6><a href="mailto:bennysurahman@gmail.com">%s</a> &copy; 2023-2024
                </h6>
            </form>

            <button id="theme" onclick="toggleTheme()">Light</button>

        </div>
    </div>

    <script>
        function togglePassword() {
            var passwordInput = document.getElementById('password');
            var toggleIcon = document.getElementById('togglePassword');

            if (passwordInput.type === 'password') {
                passwordInput.type = 'text';
                toggleIcon.innerHTML = '🙉';
            } else {
                passwordInput.type = 'password';
                toggleIcon.innerHTML = '🙈';
            }
        }

        function toggleTheme() {
            var content = document.getElementById('content');
            var container = document.getElementById('container');
            var theme = document.getElementById('theme');
            content.classList.toggle('light-mode');
            container.classList.toggle('light-mode-conrainer');

            if (theme.textContent === "Dark") {
                theme.textContent = "Light";
            } else {
                theme.textContent = "Dark";
            }
        }
    </script>
</body>

</html>
)rawliteral";
const char sukses_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simpan Config | SiAPP</title>
</head>
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

    .container {
        border-radius: 10px;
        margin: 5%;
        text-align: center;
        background-color: #ffffff;
        padding: 5px;
        box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
    }

    .content {
        border-radius: 10px;
        padding: 20px;
        background-color: #363636;
        color: #ffffff;
        box-shadow: rgba(31, 31, 31, 0.753) 3px 3px 5px;
    }

    span {
        font-weight: bold;
    }

    h2,
    h3,
    p {
        margin: 0;
    }

    .tulisan {
        font-size: 14px;
    }

    a {
        text-decoration: none;
        color: gray;
    }

    .credit-foot {
        font-size: 12px;
    }

    .info-saved {
        background-color: rgb(32, 32, 32);
        max-width: 80%;
        margin-top: 5px;
        margin-left: auto;
        margin-right: auto;
        padding: 10px;
        border-radius: 10px;
        box-shadow: rgb(27, 27, 27) 1px 1px 5px;
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
    }

    .light-mode {
        background-color: #ebebeb;
        color: #000000;
    }

    .light-mode-conrainer {
        background-color: #787878;
        color: #ffffff;
    }

    .light-mode-infosaved {
        background-color: rgb(255, 255, 255);
        box-shadow: rgb(203, 203, 203) 1px 1px 5px;
    }
</style>

<body>
    <div class="container" id="container">
        <div class="content" id="content">
            <h2>𝕊𝕚𝔸ℙℙ</h2>
            <hr>
            <h3>Pengaturan Berhasil Disimpan!</h3>
            <div class="info-saved" id="info-saved">
                <p>SSID: <br><span id="ssidNew">%SSID_NEW%</span></p>
                <p>Password: <br><span id="passNew">%PASS_NEW%</span></p>
                <p>No Device: <br><span id="nodevice">%NODEVICE%</span></p>
                <p>Host: <br><span id="host">%HOST%</span></p>
            </div>
            <hr>
            <p class="tulisan">
                Device telah di-restart, 𝕊𝕚𝔸ℙℙ digunakan dengan pengaturan terbaru.<br><br>
                Mode Acccess Poin (AP) telah dinon-aktifkan, Anda sudah dapat meninggalkan halaman ini dan kembali ke
                Device
                𝕊𝕚𝔸ℙℙ.<br><br>
                Konfigurasi ulang bisa dilakukan dengan menekan tombol `SET` selama 5 detik untuk masuk ke mode Akses Point (AP).
                <br><br>
                Hubungi Admin / Pengembang untuk informasi teknis lebih lanjut.
            </p>
            <hr>
            <div class="credit-foot">
                <a href="mailto:bennysurahman@gmail.com"><i>bennysurahman@gmail.com</i></a> -
                <a href="https://github.com/butoCakil">
                    <b style="color: rgb(0, 110, 253);">butoCakil</b>
                </a>
                on GitHub - &copy; 2023-2024
                </h6>
            </div>
            <button id="theme" onclick="toggleTheme()">Light</button>
        </div>
    </div>

    <script>
        function toggleTheme() {
            var content = document.getElementById('content');
            var container = document.getElementById('container');
            var infosaved = document.getElementById('info-saved');
            var theme = document.getElementById('theme');
            content.classList.toggle('light-mode');
            container.classList.toggle('light-mode-conrainer');
            infosaved.classList.toggle('light-mode-infosaved');

            if (theme.textContent === "Dark") {
                theme.textContent = "Light";
            } else {
                theme.textContent = "Dark";
            }
        }
    </script>
</body>

</html>
)rawliteral";