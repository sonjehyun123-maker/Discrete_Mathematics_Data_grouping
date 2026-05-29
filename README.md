# Discrete_Mathematics_Data_grouping

# 1. 프로젝트 개요

본 프로젝트는 2차원 좌표 데이터를 생성한 뒤,
거리 기반 군집화(K-Means Clustering) 알고리즘을 이용하여 데이터를 2개의 그룹으로 반복 분류하는 프로그램이다.

프로그램은:

* 랜덤 좌표 생성
* 그룹 평균 계산
* 거리 계산
* 그룹 재배치
* 반복 수렴

과정을 수행한다.

---

# 2. 전체 프로그램 흐름

```text id="l5b9sz"
랜덤 좌표 생성
↓
초기 그룹 분할
↓
평균 계산
↓
거리 계산
↓
더 가까운 그룹으로 이동
↓
새 평균 계산
↓
변화 없을 때까지 반복
```

---

# 3. 좌표 생성 방식

## 사용 이유

프로그램은:

* 0~300 범위의 좌표
* 중복 없는 랜덤 좌표

를 생성해야 한다.

단순 rand() 반복만 사용할 경우:

* 중복 검사 필요
* 반복 생성 증가
* 비효율 발생

문제가 생긴다.

따라서 본 프로젝트에서는:

```text id="kr1j3q"
Fisher-Yates Shuffle
```

기반 방식을 사용하였다.

---

# 4. Fisher-Yates Shuffle 구현 방식

## 핵심 아이디어

전체 좌표를 먼저 생성한 뒤:

```text id="pk9clu"
선택된 좌표를
다음 선택 범위 밖으로 이동
```

시키는 방식이다.

---

## 코드 구조

```c id="m8b6r4"
int r = rand() % (total_cells - i);

selected_points[i] = points[r];

points[r] = points[total_cells - 1 - i];
```

---

## 동작 과정

### 1단계

```text id="bgrgny"
전체 좌표 생성
```

```c id="tdpv6m"
Point points[WIDTH * HEIGHT];
```

배열에:

* (0,0)
* (0,1)
* (0,2)

형태로 모든 좌표 저장.

---

### 2단계

```text id="kx29qe"
랜덤 인덱스 선택
```

```c id="c9wb8n"
rand() % (total_cells - i)
```

현재 남아있는 범위 내에서만 랜덤 선택.

---

### 3단계

```text id="1ojc7d"
선택된 좌표 저장
```

```c id="xw8gpn"
selected_points[i] = points[r];
```

---

### 4단계

```text id="3c77s6"
선택된 좌표 제거
```

```c id="fclg2u"
points[r] = points[total_cells - 1 - i];
```

선택된 위치를 마지막 원소로 덮어써:

* 다음 반복에서 제외
* 중복 방지

수행.

---

## 장점

| 항목        | 설명       |
| --------- | -------- |
| 중복 제거     | 100% 보장  |
| 시간복잡도     | O(n)     |
| 랜덤 품질     | 균등 랜덤    |
| 추가 검사 불필요 | 중복 탐색 없음 |

---

# 5. GroupNode 구조체 설계

## 구조체 코드

```c id="uc4zvo"
typedef struct GroupNode {
    int x[MAX_POINTS];
    int y[MAX_POINTS];
    int c[MAX_POINTS];

    double average1[2];
    double average2[2];
} GroupNode;
```

---

## 설계 이유

좌표와 그룹 정보를 하나의 구조체에서 관리하기 위해 사용하였다.

---

## 변수 역할

| 변수       | 역할       |
| -------- | -------- |
| x[]      | x좌표 저장   |
| y[]      | y좌표 저장   |
| c[]      | 그룹 번호 저장 |
| average1 | 그룹1 평균   |
| average2 | 그룹2 평균   |

---

# 6. 초기 그룹 분할 방식

## 구현 코드

```c id="yoqq6i"
if (i < 100)
    gNode.c[i] = 1;
else
    gNode.c[i] = 2;
```

---

## 설계 이유

초기 그룹을 랜덤하게 시작하기 위해 사용하였다.

현재 좌표 배열 자체가 이미 랜덤 순서이므로:

* 앞의 100개
* 뒤의 100개

를 나누는 것만으로도 충분한 랜덤성이 발생한다.

---

## 장점

| 항목       | 설명          |
| -------- | ----------- |
| 구현 단순    | 이해 쉬움       |
| 초기 분산 가능 | 랜덤 데이터 기반   |
| 디버깅 용이   | 그룹 상태 확인 쉬움 |

---

# 7. 평균 계산 함수

## 함수 코드

```c id="ukrltj"
void calculateAverage(GroupNode* gNode)
```

---

## 사용 이유

그룹의 중심점(centroid)을 계산하기 위해 사용하였다.

K-Means 알고리즘에서:

* 평균점은 그룹 중심 역할 수행
* 이후 거리 계산 기준점이 됨

---

## 동작 방식

### 그룹1 탐색

```c id="m2qlm9"
if (gNode->c[i] == 1)
```

---

### 좌표 합산

```c id="b1d76u"
tx1 += gNode->x[i];
ty1 += gNode->y[i];
```

---

### 평균 계산

```c id="vrr6ya"
gNode->average1[0] = tx1 / (double)c1;
```

---

## 중요한 이유

반복마다 평균이 계속 바뀌므로:

* 거리 기준 변경
* 그룹 재배치 발생

하게 된다.

즉:

```text id="v98zdb"
평균이 그룹을 바꾸고
그룹이 다시 평균을 바꿈
```

구조를 가진다.

---

# 8. 거리 계산 함수

## 함수 코드

```c id="x6lccn"
double distance(int x1, int y1,
                double x2, double y2)
```

---

## 사용 이유

좌표와 그룹 평균 사이의 거리를 계산하기 위해 사용하였다.

---

## 사용 공식

---

## 구현 코드

```c id="rphq9g"
return sqrt(
    pow(x2 - x1, 2) +
    pow(y2 - y1, 2)
);
```

---

## 설계 이유

유클리드 거리는:

* 2차원 좌표 거리 계산
* 가장 일반적인 거리 공식

이기 때문에 사용하였다.

---

# 9. regroup() 함수 구조

## 함수 코드

```c id="s52sm7"
int regroup(GroupNode* gNode)
```

---

## 핵심 역할

모든 좌표를:

* 그룹1 평균
* 그룹2 평균

과 비교하여 더 가까운 그룹으로 이동시키는 역할 수행.

---

# 10. regroup() 동작 과정

## 1단계. 거리 계산

```c id="eqs3zd"
double d1 = distance(...);
double d2 = distance(...);
```

---

## 2단계. 기존 그룹 저장

```c id="2ok3lf"
int oldGroup = gNode->c[i];
```

변경 여부 확인용.

---

## 3단계. 더 가까운 그룹 선택

```c id="8k3n3x"
if (d1 < d2)
    gNode->c[i] = 1;
else
    gNode->c[i] = 2;
```

---

## 4단계. 그룹 변경 여부 검사

```c id="cx9m89"
if (oldGroup != gNode->c[i])
    changed = 1;
```

---

## 설계 이유

변경 여부를 반환함으로써:

* 더 이상 변화 없는 상태
* 즉 수렴(convergence)

판단 가능.

---

# 11. 반복 구조 설계

## 구현 구조

```c id="5v6d6f"
while (1)
{
    regroup(&gNode);

    calculateAverage(&gNode);

    if (changed == 0)
        break;
}
```

---

## 동작 방식

```text id="18rj52"
거리 계산
↓
그룹 이동
↓
새 평균 계산
↓
다시 거리 계산
```

반복 수행.

---

## 핵심 특징

현재 알고리즘은:

```text id="lpl8hs"
4 → 5 → 6 → 4 → 5 → 6
```

형태로 반복된다.

즉:

* 평균이 바뀌면
* 거리 기준도 바뀌므로
* 다시 거리 계산 필요

하다.

---

# 12. CSV 저장 함수

## 함수 코드

```c id="tazr23"
void saveCSV(GroupNode* gNode, int iteration)
```

---

## 사용 이유

반복 과정의 결과를:

* 저장
* 시각화
* 분석

하기 위해 사용하였다.

---

## 저장 구조

```csv id="t6b8op"
group,x,y
1,120,30
2,55,200
```

---

## iteration별 저장 이유

```text id="4zykrm"
iteration_0.csv
iteration_1.csv
iteration_2.csv
```

형태로 저장하여:

* 그룹 변화 과정
* 수렴 과정

확인 가능하도록 설계하였다.

---

# 13. 알고리즘의 랜덤성과 결정성

## 랜덤 요소

랜덤성은:

* 좌표 생성
* 초기 그룹 상태

에서 발생한다.

```c id="g6c0nd"
rand()
```

사용.

---

## 결정적 요소

초기 상태 이후에는:

* 거리 계산
* 평균 계산
* 그룹 재배치

가 모두 고정 규칙으로 수행된다.

즉:

* 같은 초기 데이터
* 같은 초기 그룹

이라면 항상 동일 결과가 나온다.

---

# 14. 시간 복잡도 분석

## 거리 계산

모든 좌표에 대해:

* 그룹1 거리
* 그룹2 거리

계산 수행.

---

## 시간복잡도

1회 반복:

```text id="8wqfl6"
O(n)
```

전체 반복:

```text id="iqhrrp"
O(n × iteration)
```

현재 프로젝트에서는:

* 좌표 수: 200개
* 그룹 수: 2개

이므로 일반적으로 10회 이하에서 수렴한다.

---

# 15. 결론

본 프로젝트에서는:

* 랜덤 좌표 생성
* 거리 기반 군집화
* 반복적 평균 갱신

과정을 직접 구현하였다.

특히:

* Fisher-Yates Shuffle
* 유클리드 거리 계산
* 반복 기반 재분류 구조

를 이용하여 K-Means Clustering의 핵심 원리를 구현하였다.
