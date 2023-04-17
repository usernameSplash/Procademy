# Checking Maximum Length of Backlog Queue

클라이언트 측에서 서버에 계속 데이터를 전송하면서, 서버측의 `listen` 소켓에서는 `accept`를 하지 않아 클라이언트와의 송수신 소켓을 만들지 않는다. 이렇게 하면 백로그 큐에 클라이언트측에서의 연결 요청이 쌓이게 된다.

(TCP 프로토콜의 Three Handshaking과는 무관하다. Three Handshaking은 `listen` 소켓의 `accept`와 무관하게, 클라이언트 소켓에서의 `connect`시점에 TCP API에 의해 이미 성립이 된 상태이다.)

## 확인 과정

1. listen 함수의 backlog 매개변수에 `SOMAXCONN`을 넣어본다.
    - `SOMAXCONN`의 실제 값까지 도달하지 못한다.
2. `SOMAXCONN_HINT(SOMAXCONN)`을 넣어본다.
    - 약 16000에서 소켓 에러가 발생한다.
    - 클라이언트 컴퓨터에서 서버와 연결하는 데 동적 포트(dynamic port)를 모두 사용하였기 때문에 더 이상 TCP 연결을 시도할 수 있는 포트가 없는 것이 원인이다.
3. 클라이언트에서 소켓 생성, `connect` 이후에 즉시 `closeSocket`으로 연결을 끊는다. 
    - 서버와 연결을 하자마자 `closeSocket`으로 바로 연결을 끊으면 비로소 실제 `SOMAXCONN`값만큼 백로그 큐에 연결 요청이 대기하고 있는 것을 확인할 수 있다.