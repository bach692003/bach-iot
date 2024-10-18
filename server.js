const express = require('express');
const mysql = require('mysql2');
const mqtt = require('mqtt');
const path = require('path'); // Import path module


// Khởi tạo ứng dụng Express
const app = express();
const port = 5501;

app.use(express.json());
app.use(express.static('public'));
// Serve static files from the 'templates' directory
app.use('/templates', express.static(path.join(__dirname, 'templates')));

// Kết nối đến MySQL
const db = mysql.createConnection({
    host: 'localhost',  // Địa chỉ máy chủ MySQL
    user: 'root', // Thay đổi thành tên người dùng MySQL của bạn
    password: '123456789', // Thay đổi thành mật khẩu MySQL của bạn
    database: 'mangcambien' // Tên cơ sở dữ liệu
});

// Kiểm tra kết nối MySQL
db.connect(err => {
    if (err) {
        console.error('Error connecting to MySQL:', err);
        return;
    }
    console.log('Connected to MySQL');
});

// Nhận dữ liệu cảm biến từ ESP8266 qua API
app.post('/api/sensor', (req, res) => {
    const { tem, hum, light } = req.body;

    // Câu lệnh SQL để chèn dữ liệu cảm biến
    const query = 'INSERT INTO datasensor (tem, hum, light) VALUES (?, ?, ?)';
    
    db.query(query, [tem, hum, light], (error, results) => {
        if (error) {
            console.error('Error inserting data:', error);
            return res.status(500).json({ error: 'Database insertion error' });
        }
        res.status(201).json({ message: 'Sensor data inserted', id: results.insertId });
    });
});

// Kết nối đến MQTT Broker
const mqttClient = mqtt.connect('mqtt://10.21.194.50:1885'); // Thay đổi địa chỉ IP và cổng nếu cần

// Kết nối và đăng ký chủ đề MQTT
mqttClient.on('connect', () => {
    console.log('Connected to MQTT broker');
    mqttClient.subscribe('sensor/data', (err) => {
        if (err) {
            console.error('Failed to subscribe to topic:', err);
        }
    });
});

// Xử lý dữ liệu từ MQTT
mqttClient.on('message', (topic, message) => {
    const data = JSON.parse(message.toString());
    
    const { tem, hum, light } = data;

    // Câu lệnh SQL để chèn dữ liệu cảm biến từ MQTT
    const query = 'INSERT INTO datasensor (tem, hum, light) VALUES (?, ?, ?)';
    
    db.query(query, [tem, hum, light], (error, results) => {
        if (error) {
            console.error('Error inserting data from MQTT:', error);
        } else {
            console.log('Sensor data inserted from MQTT:', results.insertId);
        }
    });
});

// API endpoint để lưu trạng thái thiết bị
app.post('/api/device', (req, res) => {
    const { device_name, state } = req.body;

    const sql = 'INSERT INTO device_history (device_name, state, timestamp) VALUES (?, ?, NOW())';
    db.query(sql, [device_name, state], (error, results) => {
        if (error) {
            console.error(error);
            return res.status(500).json({ error: 'Internal Server Error' });
        }
        res.status(200).json({ message: 'Device state saved successfully', id: results.insertId });
    });
});

// API endpoint để lấy dữ liệu cảm biến
app.get('/api/sensor', (req, res) => {
    const sqlQuery = 'SELECT * FROM datasensor ORDER BY time DESC'; // Điều chỉnh thứ tự nếu cần
    db.query(sqlQuery, (err, results) => {
        if (err) {
            console.error('Error fetching sensor data:', err);
            res.status(500).json({ error: 'An error occurred while fetching sensor data' });
            return;
        }
        res.status(200).json(results);  // Gửi kết quả dưới dạng JSON
    });
});

// API endpoint để lấy lịch sử thiết bị
app.get('/api/device_history', (req, res) => {
    const sqlQuery = 'SELECT * FROM device_history ORDER BY timestamp DESC'; // Điều chỉnh nếu cần

    db.query(sqlQuery, (err, results) => {
        if (err) {
            console.error('Error fetching device history:', err);
            res.status(500).json({ error: 'An error occurred while fetching data' });
            return;
        }
        res.status(200).json(results); // Gửi kết quả dưới dạng JSON
    });
});

// Serve the sensor data page
app.get('/sensor', (req, res) => {
    res.sendFile(__dirname + '/templates/sensor.html');
});

// Serve the device history page
app.get('/device_history', (req, res) => {
    res.sendFile(__dirname + '/templates/device_history.html');
});


// Middleware xử lý lỗi
app.use((err, req, res, next) => {
    console.error(err.stack);
    res.status(500).send('Something broke!');
});

// Bắt đầu máy chủ
app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
