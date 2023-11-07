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
        <img src="data:image/png;base64,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"
            alt="" />
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
        // function on() {
        //     var xhttp = new XMLHttpRequest();

        //     xhttp.onreadystatechange = function () {
        //         if (this.readyState == 4 && this.status == 200) {
        //             document.getElementById("statusLED").innerHTML = this.responseText
        //         }
        //     };

        //     xhttp.open("GET", "/on", true);
        //     xhttp.send();
        // }
        // function off() {
        //     var xhttp = new XMLHttpRequest();

        //     xhttp.onreadystatechange = function () {
        //         if (this.readyState == 4 && this.status == 200) {
        //             document.getElementById("statusLED").innerHTML = this.responseText
        //         }
        //     };

        //     xhttp.open("GET", "/off", true);
        //     xhttp.send();
        // }

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