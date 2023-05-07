# Ringbuffer

동적 길이를 갖는 원형 큐 형태의, 자료를 바이트 단위로 관리하는 자료구조 Ringbuffer를 만들었다.

초기 용량은 512 Bytes로 설정하였고, 용량을 초과하는 Enqueue 연산 시 최대 8배까지 늘어날 수 있도록 작성하였다.

최대 용량을 초과하면 Enqueue에 실패했다는 의미로 0을 반환한다.

## 기본 연산
1. `Enqueue` : Ringbuffer의 rear 위치에 원하는 byte만큼 입력 
2. `Peek` : Ringbuffer의 front 위치로부터 원하는 byte만큼 데이터 복사
3. `Dequeue` : Ringbuffer의 front 위치로부터 원하는 byte만큼 데이터 삭제

## Mistake
Enqueue 연산을 할 때 Enqueue를 원하는 크기보다 rear 포인터에서부터 Ringbuffer의 끝까지의 크기가 더 작을 때 데이터를 잘라서 넣어야 한다. (`DirectEnqueueSize` 함수 참고)

Enqueue 연산을 할 때 현재 용량을 초과하게 되면 메모리 재할당을 해야 하는데(`Reserve` 함수) `DirectEnqueueSize` 함수도 바뀐 용량을 대상으로 계산하도록 호출해주었어야 했는데, 호출 순서를 잘못 작성하여 이전 용량을 대상으로 계산하게 되는 실수가 있었다.

## Test
Enqueue를 3번 연속 하고 Dequeue를 3번 연속 진행한다. 각 Enqueue 시에는 무작위 숫자를 추출하여 해당 숫자만큼의 Byte 만큼 자료를 입력하고, Dequeue 시에는 Enqueue한 만큼 진행한다.   

Ringbuffer에 Enqueue한 결과와, Ringbuffer로부터 Dequeue한 결과가 일치하는지 확인한다.