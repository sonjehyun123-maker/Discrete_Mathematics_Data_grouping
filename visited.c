int visited[WIDTH][HEIGHT] = {0};

int count = 0;

while (count < MAX_POINTS) {

    int x = rand() % WIDTH;
    int y = rand() % HEIGHT;

    // 아직 사용 안 된 좌표
    if (visited[x][y] == 0) {

        visited[x][y] = 1;

        selected_points[count].x = x;
        selected_points[count].y = y;

        count++;
    }
}

/*
    visited 배열 기반 중복 제거 방식

    visited[x][y]

    0 : 아직 사용되지 않은 좌표
    1 : 이미 사용된 좌표

    랜덤 좌표 생성 후
    visited 배열을 통해 즉시 중복 여부를 검사한다.
*/