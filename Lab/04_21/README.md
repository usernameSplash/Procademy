# Echo Server

클라이언트에서 보낸 메시지를 그대로 돌려주는 에코 서버이다.

C++ 윈도우 소켓의 데이터 전송 단위의 자료형이 `char*` 이기 때문에, 유니코드 문자열을 보내고 받을 때 추가 처리가 필요하다.

## TCP Echo Server
![image](TCP_Echo.png)