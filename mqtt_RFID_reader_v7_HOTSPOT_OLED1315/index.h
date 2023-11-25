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
            background-color: #d5f2d6;
        }

        h3 {
            text-align: center;
            font-weight: bold;
        }

        input[type=text],
        [type=password],
        select {
            width: 100%;
            padding: 12px 20px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
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
        }

        input[type=submit]:hover {
            background-color: #45A049;
        }

        .container {
            border-radius: 10px;
            background-color: #4d4242;
            padding: 5px;
            margin: 10%;
        }

        .container form {
            background-color: #f2f2f2;
            padding: 20px;
            border-radius: 10px;
        }

        .inputPass {
            display: flex;
        }

        span {
            background-color: #cccccc;
            padding: 5px;
            color: aliceblue;
            border-radius: 5px;
            margin: auto 10px;
            height: 30px;
            box-shadow: #4d4242 1px 1px 3px;
        }

        span:hover{
            cursor: pointer;
        }
    </style>


    <div class="container">
        <form action="/action_page" method="post">
            <h3>Konfigurasi SiAPP Device Scanner ID Card %s</h3>
            <label for="ssid">SSID</label>
            <input type="text" id="ssid" name="ssidNew" value="%SSID_NEW%" placeholder="SSID WiFi Anda" required>

            <label for="password">Password</label>
            <div class="inputPass">
                <input type="password" id="password" name="passNew" value="%PASS_NEW%" placeholder="Kosongkan jika tanpa password">
                <span id="togglePassword" onclick="togglePassword()">
                    🙈
                </span>
            </div>

            <label for="nodevice">Nomor Device</label>
            <input type="text" id="nodevice" name="nodevice" value="%NODEVICE%" placeholder="Nomor yang telah terdaftar" required>

            <label for="host">HOST</label>
            <input type="text" id="host" name="host" value="%HOST%" placeholder="Alamat Host" required>

            <input type="submit" value="Simpan">
        </form>
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
    }

    .container {
        border-radius: 10px;
        margin: 5%;
        text-align: center;
        background-color: #746c6c;
        padding: 5px;
    }
    
    .content {
        border-radius: 10px;
        padding: 5px;
        background-color: #f0f0f0;
    }

    span {
        font-weight: bold;
    }
</style>

<body>
    <div class="container">
        <div class="content">
            <h3>Pengaturan Berhasil Disimpan!</h3>
            <hr>
            <p>SSID: <span id="ssidNew">%SSID_NEW%</span></p>
            <p>Password: <span id="passNew">%PASS_NEW%</span></p>
            <p>No Device: <span id="nodevice">%NODEVICE%</span></p>
            <p>Host: <span id="host">%HOST%</span></p>
            <hr>
            <p>
                Device telah di-restart, SiAP digunakan dengan pengaturan terbaru. <br>
                Periksa apakah Konfigurasi Device telah berkerja sesuai dengan yang diharapkan. <br><br>
                Ulangi Konfigurasi dengan menekan tombol `SET` selama 10 detik (10 kali beep) untuk masuk ke mode Akses
                Point (AP).
                <br><br>
                Hubungi Admin / Pengembang untuk informasi teknis lebih lanjut.
                <hr>
                <b>butoCakil</b> on GitHub - <i>bennysurahman@gmail.com</i>
            </p>
        </div>
    </div>
</body>

</html>
)rawliteral";