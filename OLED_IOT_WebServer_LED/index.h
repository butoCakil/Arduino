static const char MAIN_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>iot Control</title>
</head>

<body>
    <div align="center">
        <h1>Ini Program nodeMCU</h1>
        <!-- image to data URI -->
       <img src="data:image/png;base64,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" alt="" style="width: 100px"/>
        <br>
        <hr>
        <h6>Status LED:
            <span id="statusLED">NULL</span>
        </h6>

        <!-- <button onclick="on();">
            <h3>ON</h3>
        </button>
        <button onclick="off();">
            <h3>OFF</h3>
        </button> -->

        <!-- Rounded switch -->
        <label class="switch">
            <input type="checkbox" id="led" onchange="ledButton('led')">
            <span class="slider round"></span>
        </label>

        <style>
            /* The switch - the box around the slider */
            .switch {
                position: relative;
                display: inline-block;
                width: 60px;
                height: 34px;
            }

            /* Hide default HTML checkbox */
            .switch input {
                opacity: 0;
                width: 0;
                height: 0;
            }

            /* The slider */
            .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: #ccc;
                -webkit-transition: .4s;
                transition: .4s;
            }

            .slider:before {
                position: absolute;
                content: "";
                height: 26px;
                width: 26px;
                left: 4px;
                bottom: 4px;
                background-color: white;
                -webkit-transition: .4s;
                transition: .4s;
            }

            input:checked+.slider {
                background-color: #2196F3;
            }

            input:focus+.slider {
                box-shadow: 0 0 1px #2196F3;
            }

            input:checked+.slider:before {
                -webkit-transform: translateX(26px);
                -ms-transform: translateX(26px);
                transform: translateX(26px);
            }

            /* Rounded sliders */
            .slider.round {
                border-radius: 34px;
            }

            .slider.round:before {
                border-radius: 50%;
            }
        </style>
    </div>
    <script>
        function ledButton(id) {
            const button = document.getElementById(id);
            if(button.checked){
                var url = "/on";
            } else {
                var url = "/off";
            }

            var xhttp = new XMLHttpRequest();

            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("statusLED").innerHTML = this.responseText
                }
            };

            xhttp.open("GET", url, true);
            xhttp.send();
        }
    </script>
</body>

</html>
)=====";