# Online Painting Client with WSAAsyncSelect

WSAAsyncSelect 모델을 사용하여, 제공된 서버에 접속해 다같이 그림을 그릴 수 있는 클라이언트 프로그램을 제작한다.

[Lab_03_29](https://github.com/usernameSplash/Procademy/tree/main/Lab/03_29) 프로그램에 네트워크를 도입한 것과 같다.

마우스 왼쪽 버튼을 누른 채로 이동하였을 때 곧바로 그리는 작업(`MoveToEx`, `LineTo`)을 하는 대신 서버에 시작 좌표와 도착 좌표를 보낸다.

클라이언트는 오직 서버로부터 받은 패킷의 좌표를 통해서만 윈도우에 그림을 그린다.