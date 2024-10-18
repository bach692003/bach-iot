### 📌 GIỚI THIỆU ĐỀ TÀI

SỬ DỤNG ESP8266 ĐO NHIỆT ĐỘ , ĐỘ ẨM, ÁNH SÁNG, ĐIỀU KHIỂN THIẾT BỊ THÔNG QUA MQTT

### 📌 Lý do chọn đề tài

Internet of Things (IoT) là xu hướng đang được các doanh nghiệp trong lĩnh vực công nghệ quan tâm và đầu tư nghiên cứu. Cuộc đua IoT đã và đang diễn ra mạnh mẽ giữa các doanh nghiệp trên toàn thế giới. Theo Gartner, đến năm 2020, thế giới sẽ có khoảng 20 tỷ thiết bị sử dụng IoT, doanh số dự kiến trong năm là 437 tỷ USD. Nắm bắt được nhu cầu đó và kết hợp yêu cầu của môn học nhóm chúng em đã cho phát triển một ứng dụng IoT là một hệ thống điều khiển giám sát đo nhiệt độ, độ ẩm, ánh sáng.  Hệ thống được thiết kế hợp lý với mong muốn được tiếp cận đến với nhiều người dùng nhất có thể.

### 📌 Mô tả nội dung đề tài

Xây dựng hệ thống IOT sử dụng ESP8266 để đo nhiệt độ, độ ẩm, ánh sáng với cảm biến DHT11 và BH1750 thông qua mosquitto MQTT Broker, sau đó dùng MySQL để lưu giá trị thành cơ sở dữ liệu.

### 📌 Chức năng chung của ứng dụng

Hệ thống sẽ nhận dữ liệu về nhiệt độ, độ ẩm và ánh sáng từ server gửi về, lưu trữ dữ liệu vào database đồng thời xử lí dữ liệu đó với các ngưỡng mà người dùng đã thiết lập.

### 📌 Các thiết bị sử dụng:
1. Vi xử lý ESP8266
2.  Cảm biến nhiệt độ, độ ẩm DHT11
3.  Cảm biến ánh sáng BH1750
4.  LED đơn
5.  Dây nối , rắc cắm

### 📌 Các công cụ được sử dụng:
1. Visual Code
2. Phần mềm lập trình Arduino
3. Note Pad ++
4. MySQL 8.4
5. Command Prompt

### 📌 Hướng Dẫn Cài Đặt và Chạy Dự Án
1. Cài Đặt Môi Trường
   
	1.1. Cài Đặt Node.js

	Tải Node.js từ trang web chính thức và cài đặt nó trên máy tính.


	1.2. Cài Đặt MySQL
	
	Tải và cài đặt MySQL từ trang web chính thức.

	Sau khi cài đặt, tạo một cơ sở dữ liệu với tên mangcambien.

2. Cài Đặt Thư Viện Cần Thiết

	2.1. Tạo dự án Node.js
   
	Mở Terminal (hoặc Command Prompt) và tạo một thư mục mới cho dự án:
   
		mkdir iot-project
		cd iot-project

	Khởi tạo dự án Node.js
	
 		npm init -y

	2.2. Cài đặt các thư viện:

		npm install express mysql2 mqtt

3. Cấu hình dự án

   3.1 Tạo File Chạy Server

	Tạo một file server.js trong thư mục dự án viết code.


   3.2. Cấu hình cơ sở dữ liệu

	 Mở MySQL và tạo hai bảng trong cơ sở dữ liệu mangcambien:

			CREATE TABLE datasensor (
    	id INT AUTO_INCREMENT PRIMARY KEY,
    	tem FLOAT,
   		 hum FLOAT,
    	light FLOAT,
    	time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
			);

			CREATE TABLE device_history (
    	id INT AUTO_INCREMENT PRIMARY KEY,
    	device_name VARCHAR(50),
    	state VARCHAR(50),
    	timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
			);

4. Cài đặt ESP8266 với Arudino

	4.1 Cài đặt Arudino

	 Tải và cài đặt
	
 		Arduino IDE

  4.2 Thêm Board ESP8266


  Mở Arduino IDE, vào File -> Preferences, thêm vào phần "Additional Board Manager URLs":

	 http://arduino.esp8266.com/stable/package_esp8266com_index.json

 Sau đó  vào Tools -> Board -> Board Manager, tìm và cài đặt board ESP8266.

 4.3 Cài đặt các thư viện cần thiết

 Cài đặt các thư viện
 
PubSubClient (cho MQTT)
DHT (cho cảm biến DHT11)
BH1750 (cho cảm biến ánh sáng)

4.4 Cập nhật thông tin kết nối wifi và mqtt

Trong mã nguồn của ESP8266, cập nhật thông tin kết nối:

	const char* ssid = "Tên_WiFi_Của_Bạn";
	const char* password = "Mật_Khẩu_WiFi_Của_Bạn";
	const char* mqtt_server = "Địa_Chỉ_IP_Broker_MQTT";

 5. Chạy Dự Án

	5.1 Chạy Server Node.js

	Trong Terminal , chạy lệnh sau để khởi động server:

		node server.js

	Server sẽ chạy trên
		
		http://localhost:5501

	5.2 Tải lên ESP8266

	Kết nối ESP8266 với máy tính và tải mã nguồn lên board.

	6. Kiểm tra hoạt động
    
	Mở trình duyệt và truy cập vào
	 
		http://localhost:5501/sensor
	để xem dữ liệu cảm biến.

	Truy cập vào
	
 		http://localhost:5501/device_history
	để xem lịch sử trạng thái thiết bị.

### 📌 Note:

## 📚 Tài liệu tham khảo

	 https://hocarm.org/mqtt-client-va-mqtt-broker/
	https://www.proe.vn/nodemcu-esp8266-esp12e


	

   
   

   
