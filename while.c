    /*
        while문 기반 중복 없는 랜덤 좌표 생성

        랜덤 x, y 좌표를 생성한 뒤
        이미 저장된 좌표와 비교하여

        중복이 아닐 경우에만 저장한다.

        총 200개의 좌표가 생성될 때까지 반복 수행.
    */

    int count = 0;

    while (count < MAX_POINTS) {

        // 0 ~ 300 범위 랜덤 x좌표 생성
        int x = rand() % WIDTH;

        // 0 ~ 300 범위 랜덤 y좌표 생성
        int y = rand() % HEIGHT;

        // 중복 여부 검사 변수
        int duplicate = 0;

        /*
            현재까지 저장된 좌표들과 비교

            같은 x, y 좌표가 존재하면
            duplicate = 1 설정
        */

        for (int i = 0; i < count; i++) {

            if (selected_points[i].x == x &&
                selected_points[i].y == y)
            {
                duplicate = 1;
                break;
            }
        }

        /*
            중복이 아닐 경우만 저장

            duplicate == 0
            → 새로운 좌표
        */

        if (!duplicate) {

            selected_points[count].x = x;
            selected_points[count].y = y;

            count++;
        }
    }