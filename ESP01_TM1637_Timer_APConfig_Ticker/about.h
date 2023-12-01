const char selesai_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CONTROLER</title>
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

        .container {
            margin: 10%;
            padding: 5px;
            border-radius: 10px;
            background-color: #ffffff;
            box-shadow: #363636 5px 7px 10px;
        }

        .content {
            padding: 20px;
            border-radius: 10px;
            background-color: #363636;
            color: #ffffff;
            box-shadow: rgba(48, 39, 71, 0.753) 3px 3px 5px;
        }

        h2 {
            text-align: center;
            margin: 0;
        }

        p {
            text-align: center;
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
            text-align: center;
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
            <h2>CONTROLER</h2>
            <hr>
            <p class="tulisan">
                Device telah di-restart.
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