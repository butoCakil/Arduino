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
        }

        input[type=text],
        [type=password],
        [type=nodevice],
        [type=host],
        select
         {
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

        div {
            border-radius: 5px;
            background-color: #f2f2f2;
            padding: 20px
        }
    </style>

    <h3>Konfigurasi SiAPP Device Scanner ID Card</h3>

    <div>
        <form action="/action_page">
            <label for="ssid">SSID</label>
            <input type="text" id="ssid" name="ssidNew" placeholder="SSID WiFi Anda">

            <label for="password">Password</label>
            <input type="password" id="password" name="passNew" placeholder="Kosongkan jika tidak menggunakan password">

            <label for="hotspot">Mode Hotspot Portal</label>
            <select name="hotspot" id="hotspot">
                <option value="">Tidak</option>
                <option value="hotspot">Ya</option>
            </select>

            <label for="nodevice">Nomor Device</label>
            <input type="text" id="nodevice" name="nodevice"
                placeholder="Nomor Device Harus diisi dengan nomor yang valid">

            <label for="host">HOST</label>
            <input type="text" id="host" name="host" placeholder="Alamat Host">

            <input type="submit" value="Simpan">
        </form>
    </div>
</body>

</html>
)rawliteral";
const char sukses_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Save Config</title>
</head>
<style>
    div {
        position: fixed;
        top: 50%;
        left: 50%;
        margin-top: -100px;
        margin-left: -300px;
    }
</style>

<head>
    <title>Berhasil | SiAPP</title>
</head>

<body>
    <div>
        <h1>Pengaturan Berhasil Disimpan!</h1>
    </div>
</body>

</html>
)rawliteral";