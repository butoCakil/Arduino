const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login CONTROLER</title>
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
            padding: 5px;
            border-radius: 10px;
            /* background-color: #555; */
            background-color: #dfdfdf;
            box-shadow: rgba(48, 39, 71, 0.753) 2px 2px 10px;
        }

        .light-mode-conrainer {
            background-color: #787878;
            color: #ffffff;
        }

        .content {
            padding: 20px;
            border-radius: 10px;
            background-color: #363636;
            color: #ffffff;
            /* background: linear-gradient(to top, rgb(255, 255, 255), rgb(255, 255, 255)); */
            box-shadow: rgba(48, 39, 71, 0.753) 3px 3px 5px;
        }

        .light-mode {
            background-color: #ebebeb;
            color: #000000;
        }

        h2,
        h6 {
            text-align: center;
            margin: 0;
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
        }

        a {
            text-decoration: none;
            color: gray;
            text-shadow: rgba(51, 27, 27, 0.753) 2px 2px 10px;
        }

        .tmblreboot {
            display: flex;
            align-items: center;
            justify-content: center;
            margin-top: 10px;
            color: rgb(243, 11, 11);
        }

        .tmblreboot:hover {
            font-weight: bold;
        }

        span {
            background-color: #ffffff;
            padding: 5px;
            color: aliceblue;
            border-radius: 0 5px 5px 0;
            margin: auto auto;
            height: 31px;
            /* box-shadow: rgba(255, 255, 255, 0.753) 2px 2px 3px; */
        }

        span:hover {
            cursor: pointer;
            box-shadow: #a28787 1px 1px 3px;
        }

        .inputPass {
            display: flex;
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
    </style>
    <div class="container" id="container">
        <div class="content" id="content">
            <h2>BILLING CONTROL</h2>
            <h6>Config</h6>
            <hr>
            <h2>Sign in</h2>
            <br>
            <form action="/home" method="get">
                <input type="hidden" name="idchip" value="%s">
                <div class="field-wrapper">
                    <label>username</label>
                    <input type="text" name="username" placeholder="username" required>
                </div>
                <div class="field-wrapper">
                    <label>password</label>
                    <div class="inputPass">
                        <input type="password" id="password" name="password" placeholder="password" required>
                        <span id="togglePassword" onclick="togglePassword()">
                            🙈
                        </span>
                    </div>
                </div>

                <div class="field-wrapper">
                    <input type="submit" value="Login">
                </div>
            </form>

            <a class="tmblreboot" href="/reboot"
                onclick="return confirm('Device akan direstart dalam 3 detik..');">Reboot Device&nbsp;&#8634;</a>
            <hr>
            <h6><a href="mailto:bennysurahman@gmail.com">%s</a> &copy; 2023-2024
            </h6>
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