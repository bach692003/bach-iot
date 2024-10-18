<script>
    // Kết nối đến MQTT broker
    const mqttBroker = "10.21.194.50"; // Địa chỉ IP của broker MQTT
    const mqttPort = 9002; // Cổng MQTT
    const username = "bach"; // Tên người dùng nếu có
    const password = "123"; // Mật khẩu nếu có
    const clientId = "webClient_" + Math.random().toString(16).substr(2, 8); // ID của client

    // Tạo client MQTT
    const client = mqtt.connect(`ws://${mqttBroker}:${mqttPort}`, {
        username: username,
        password: password
    });

    // Thiết lập biểu đồ với Chart.js
    const ctx = document.getElementById('sensorChart').getContext('2d');
    const sensorChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Nhiệt độ (°C)',
                    data: [],
                    borderColor: 'red',
                    borderWidth: 2,
                    fill: false
                },
                {
                    label: 'Độ ẩm (%)',
                    data: [],
                    borderColor: 'green',
                    borderWidth: 2,
                    fill: false
                },
                {
                    label: 'Cường độ ánh sáng (lux)',
                    data: [],
                    borderColor: 'yellow',
                    borderWidth: 2,
                    fill: false
                }
            ]
        },
        options: {
            responsive: true,
            scales: {
                x: {
                    type: 'time',
                    time: {
                        unit: 'second' // Hiển thị dữ liệu theo giaay
                    }
                },
                y: {
                    beginAtZero: true
                }
            }
        }
    });

    // Khi kết nối thành công
    client.on('connect', function () {
        console.log('Connected to MQTT broker');
        client.subscribe('sensor/data');  // Đăng ký topic để nhận dữ liệu cảm biến
        client.subscribe("control/light"); // Đăng ký các chủ đề điều khiển
        client.subscribe("control/fan");
        client.subscribe("control/ac");
    });

    // Khi nhận được dữ liệu từ topic đã đăng ký
    client.on('message', function (topic, message) {
        if (topic === 'sensor/data') {
            const data = JSON.parse(message.toString());

            // Hiển thị dữ liệu lên trang HTML
            document.getElementById('data_temperature').innerHTML = data.temperature + ' °C';
            document.getElementById('data_humidity').innerHTML = data.humidity + ' %';
            document.getElementById('data_light').innerHTML = data.light + ' Lux';

            // Cập nhật biểu đồ
            const currentTime = new Date();
            sensorChart.data.labels.push(currentTime);
            sensorChart.data.datasets[0].data.push(data.temperature); // Nhiệt độ
            sensorChart.data.datasets[1].data.push(data.humidity);    // Độ ẩm
            sensorChart.data.datasets[2].data.push(data.light);       // Cường độ ánh sáng

            // Giới hạn số lượng dữ liệu trên biểu đồ
            if (sensorChart.data.labels.length > 20) {
                sensorChart.data.labels.shift();
                sensorChart.data.datasets[0].data.shift();
                sensorChart.data.datasets[1].data.shift();
                sensorChart.data.datasets[2].data.shift();
            }

            sensorChart.update();
        }

        // Xử lý điều khiển thiết bị
        if (topic.startsWith("control/")) {
            const device = topic.split('/')[1]; // Lấy tên thiết bị
            const status = message.toString(); // Lấy trạng thái từ thông điệp

            // Cập nhật trạng thái cho giao diện người dùng
            const statusSpan = document.getElementById(`${device}Status`);
            statusSpan.innerText = (status === 'ON') ? 'On' : 'Off';
        }
    });

    // Hàm điều khiển thiết bị
    function toggleDevice(buttonId, device) {
    const statusSpan = document.getElementById(device + 'Status');
    const button = document.getElementById(buttonId);

    // Lấy trạng thái hiện tại của thiết bị
    const currentStatus = statusSpan.innerText;

    // Đảo ngược trạng thái
    const newStatus = currentStatus === "On" ? "Off" : "On";
    statusSpan.innerText = newStatus;

    // Gửi lệnh qua MQTT
    const command = newStatus === "On" ? "ON" : "OFF";
    const topic = `control/${device}`;
    client.publish(topic, command);

    // Thay đổi văn bản và lớp của nút
    if (newStatus === "On") {
        button.innerText = "Tắt";
        button.classList.remove('off');
        button.classList.add('on');
    } else {
        button.innerText = "Bật";
        button.classList.remove('on');
        button.classList.add('off');
    }
}

</script>