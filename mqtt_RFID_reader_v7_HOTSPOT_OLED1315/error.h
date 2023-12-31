const char error_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ERROR</title>
</head>

<body>
    <div class="content">
        <h1>Error</h1>
        <div>
            <p>%s</p>
        </div>
        <div>
            <a href="%c">Kembali</a>
        </div>
    </div>

    <style>
        body {
            font-family: 'Courier New', Courier, monospace;
            margin: 0;
            padding: 0;
            height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            background: linear-gradient(to top, rgb(0, 162, 255), aqua);
            text-align: center;
        }

        .content {
            display: flex;
            flex-direction: column;
            word-wrap: break-word;
        }

        h1{
            font-size: 48px;
        }

        a {
            text-decoration: none;
            background-color: #31a037;
            padding: 5px;
            border-radius: 5px;
            box-shadow: darkcyan 2px 2px 10px;
            color: rgb(255, 255, 255);
            border: white 2px solid;
        }
        
        a:hover {
            background-color: #15701a;
            box-shadow: darkcyan 1px 1px 5px;
        }
    </style>
</body>

</html>
)rawliteral";