#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 301
#define HEIGHT 301
#define MAX_POINTS 200 // 추출할 좌표 개수

typedef struct GroupNode {
    int x[MAX_POINTS];  // x좌표 저장
    int y[MAX_POINTS];  // y좌표 저장
    int c[MAX_POINTS];  // 그룹 번호 저장 (1 또는 2)

    double average1[2]; // 그룹1 평균
    double average2[2]; // 그룹2 평균
} GroupNode;

// 좌표를 나타내는 구조체
typedef struct {
    int x;
    int y;
} Point;

// 거리 계산 함수
double distancePoint(int x1, int y1,
                     double x2, double y2)
{
    return sqrt(
        pow(x2 - x1, 2) +
        pow(y2 - y1, 2)
    );
}

// 평균 계산 함수
void calculateAverage(GroupNode* gNode)
{
    int tx1 = 0, ty1 = 0; // 그룹1 좌표 합
    int tx2 = 0, ty2 = 0; // 그룹2 좌표 합

    int c1 = 0, c2 = 0;   // 그룹별 좌표 개수

    for (int i = 0; i < MAX_POINTS; i++) {

        // 그룹1
        if (gNode->c[i] == 1) {

            tx1 += gNode->x[i];
            ty1 += gNode->y[i];

            c1++;
        }

        // 그룹2
        else {

            tx2 += gNode->x[i];
            ty2 += gNode->y[i];

            c2++;
        }
    }

    // 그룹1 평균 계산
    gNode->average1[0] = tx1 / (double)c1;
    gNode->average1[1] = ty1 / (double)c1;

    // 그룹2 평균 계산
    gNode->average2[0] = tx2 / (double)c2;
    gNode->average2[1] = ty2 / (double)c2;
}

// 그룹 재배치 함수
int regroup(GroupNode* gNode)
{
    int changed = 0;

    for (int i = 0; i < MAX_POINTS; i++) {

        // 그룹1 평균과 거리 계산
        double d1 = distancePoint(
            gNode->x[i],
            gNode->y[i],
            gNode->average1[0],
            gNode->average1[1]
        );

        // 그룹2 평균과 거리 계산
        double d2 = distancePoint(
            gNode->x[i],
            gNode->y[i],
            gNode->average2[0],
            gNode->average2[1]
        );

        // 이전 그룹 저장
        int oldGroup = gNode->c[i];

        // 더 가까운 그룹으로 이동
        if (d1 < d2)
            gNode->c[i] = 1;
        else
            gNode->c[i] = 2;

        // 그룹 변경 여부 확인
        if (oldGroup != gNode->c[i])
            changed = 1;
    }

    return changed;
}

// CSV 저장 함수
void saveCSV(GroupNode* gNode, int iteration)
{
    char filename[50];

    // iteration 번호에 따라 파일 이름 생성
    // 예: iteration_0.csv
    sprintf(filename, "iteration_%d.csv", iteration);

    // 파일 쓰기 모드로 열기
    FILE* fp = fopen(filename, "w");

    // 파일 열기 실패 검사
    if (fp == NULL) {

        printf("파일 열기 실패\n");
        return;
    }

    // CSV 헤더 작성
    fprintf(fp, "group,x,y\n");

    // 그룹1 먼저 저장
    for (int i = 0; i < MAX_POINTS; i++) {

        // 그룹1만 출력
        if (gNode->c[i] == 1) {

            fprintf(fp, "%d,%d,%d\n",
                gNode->c[i], // 그룹 번호
                gNode->x[i], // x좌표
                gNode->y[i]  // y좌표
            );
        }
    }
    
    // 그룹 2 저장
    for (int i = 0; i < MAX_POINTS; i++) {

        // 그룹2만 출력
        if (gNode->c[i] == 2) {

            fprintf(fp, "%d,%d,%d\n",
                gNode->c[i], // 그룹 번호
                gNode->x[i], // x좌표
                gNode->y[i]  // y좌표
            );
        }
    }

    // 파일 닫기
    fclose(fp);

    // 저장 완료 메시지 출력
    printf("%s 저장 완료\n", filename);
}

int main(void)
{
    Point points[WIDTH * HEIGHT];
    Point selected_points[MAX_POINTS];

    int index = 0;

    GroupNode gNode;

    // 1. 모든 가능한 좌표(X, Y)를 배열에 초기화
    for (int y = 0; y < HEIGHT; y++) {

        for (int x = 0; x < WIDTH; x++) {

            points[index].x = x;
            points[index].y = y;

            index++;
        }
    }

    // 2. 난수 생성기 초기화
    srand((unsigned int)time(NULL));

    // 3. 피셔-예츠 셔플(Fisher-Yates Shuffle)을 활용한 중복 없는 추출
    int total_cells = WIDTH * HEIGHT;

    for (int i = 0; i < MAX_POINTS; i++) {

        // [핵심 1]
        // 아직 선택되지 않은 범위 안에서 랜덤 인덱스 선택
        int r = rand() % (total_cells - i);

        // 랜덤 좌표 저장
        selected_points[i] = points[r];

        // [핵심 2]
        // 선택된 좌표를 마지막 위치 값으로 덮어쓰기
        // 다음 랜덤 범위에서 제외되어 중복 방지
        points[r] = points[total_cells - 1 - i];
    }

    /*
            [ 아직 안 뽑힌 구역 ]         │         [ 이미 뽑힌 구역 ]
            (언제든 뽑힐 수 있음)         │    (난수 범위에서 제외된 데드존)
    ─────────────────────────────────────────
      points[0]  ...  points[마지막-1-i]  │    points[마지막-i] ... points[끝]
                                          │
                     ▲                   │                 ▲
            이 안에서만 rand() 발생       │    처음 뽑힌 녀석들이 뒤에서부터 채워짐
    */

    // 초기 그룹 분할
    for (int i = 0; i < MAX_POINTS; i++) {

        // 좌표 저장
        gNode.x[i] = selected_points[i].x;
        gNode.y[i] = selected_points[i].y;

        // 그룹1
        if (i < 100)
            gNode.c[i] = 1;

        // 그룹2
        else
            gNode.c[i] = 2;
    }

    // 초기 평균 계산
    calculateAverage(&gNode);

    // 초기 평균 출력
    printf("초기 평균\n");

    printf("그룹1 평균 : (%lf, %lf)\n",
        gNode.average1[0],
        gNode.average1[1]);

    printf("그룹2 평균 : (%lf, %lf)\n",
        gNode.average2[0],
        gNode.average2[1]);

    // 초기 CSV 저장
    saveCSV(&gNode, 0);

    int iteration = 1;

    while (1) {

        printf("\n===== Iteration %d =====\n", iteration);

        // 그룹 재배치
        int changed = regroup(&gNode);

        // 평균 재계산
        calculateAverage(&gNode);

        // 평균 출력
        printf("그룹1 평균 : (%lf, %lf)\n",
            gNode.average1[0],
            gNode.average1[1]);

        printf("그룹2 평균 : (%lf, %lf)\n",
            gNode.average2[0],
            gNode.average2[1]);

        // CSV 저장
        saveCSV(&gNode, iteration);

        // 변화 없으면 종료
        if (!changed) {

            printf("\n수렴 완료\n");
            break;
        }

        iteration++;
    }

    return 0;
}