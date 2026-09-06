# DIY_FND_Clock 🕙
STM32F103C8T6 공부를 위한 FND 디지털 시계 DIY 프로젝트!

"이 프로젝트를 만약 보시고 있다면, 피드백이나 더 좋은 의견이 있으신 분들은 언제든 편하게 이슈(Issues)나 댓글 남겨주시면 감사하겠습니다 😃"

<img width="625" height="969" alt="FND_Clock_시계" src="https://github.com/user-attachments/assets/3c6351f6-9d52-420a-b717-b5b612081459" />

## 목적
- 다이나믹 제어: GPIO 핀 사용을 최소화하기 위해 FND 다이내믹 제어 방식 적용.
- VBAT 백업 전원 설계: CR2032 / CR2025 코인셀 배터리와 LSE RTC를 활용하여 주 전원이 차단되어도 시간 데이터 유지.

## Hardware Circuit Diagram
1. 상세 회로도(PDF) 📌 [FND_Clock_Vr0.1_sch.pdf](./FND_Clock_Vr0.1_sch.pdf)  
2. Gerber file 📌 [Gerber.zip](./Gerber.zip)

<img width="416" height="518" alt="PCB 사진" src="https://github.com/user-attachments/assets/2fc71a1c-5156-4927-b2a8-aff35ef93e96" />

## 상태 천이도
<img width="951" height="352" alt="FND_Clock_Flow drawio" src="https://github.com/user-attachments/assets/e5278c5e-c5e5-408f-80ac-fc591287e5bf" />

동작 설명)
1. Power ON / Normal Mode
   > 전원이 투입되면, LSE RTC 기반의 현재 시간을 FND에 0.5초 주기마다 읽어와 표시.
   > short 누름은 무시. ( 추후 다른 페이즈로 넘어가도록 하여 기능 추가 가능.)
2. Enter Time Setting Mode
   > long 누름 입력시 RTC 시간을 읽어와 임시로 set_time에 저장하고 MODE_SET_HOUR 모드로 진입.
   > short 누름시 '시'값을 1씩 올림 원하는 값이 도달시 long 누름 후 빠져 나옴
   > 같은 방식으로 '분'도 설정하고 나옴
3. Apply
   > 마지막 MODE_SET_MIN에서 long 누름 후 빠져나오면 변경된 시간을 RTC register에 갱신 후 MODE_NORMAL로 복귀

## 💡 Developer's Reflection (개발 회고)
- 생각보다 내가 아는게 많지 않구나..
  > 이번 프로젝트를 진행하며, STM32같은 MCU를 이용해서 회로 설계시 EVM 회로도를 참고해서 그려야한다는 것과, 회로도를 그릴 때 DataSheet의 pin descripton을 보고 선택해서 그리는 것이 아닌 CubeMX 이용해서 먼저 사용하고자하는 핀을 설정 한 후에
    그에 맞춰 회로 설계가 진행되어야한다는 것을 이해하게 되었습니다. 추가로, 시계를 만들다 보니 Xtal에 대한 내용도 찾아보게 되었고 생각보다 Xtal 기술이 많은 변화가 있었고(리소그래피 기술을 통한 사이즈 감소) 정확도도 굉장히 높다는 것([ppm] 오차)과
    Xtal같은 걸 측정하는 것에대한 의문점을 가지게 되었다.
    Xtal 발진을 위한 load_cap의 용량 산출 방법과 제대로 동작되고 있는지 측정하는 것 load_cap의 용량에 20pF 정도 밖에 안되다 보니 오실로스코프의 Probe를 가져다 대는 순간 "허상의 파형을 볼 수도 있겠구나"라는 생각과 이를 해결하기 위해서는 Passive Probe가 아닌
    Active Probe를 사용해야 정확하게 볼 수 있다는 것을 알고 생각보다 "제대로 됨"이라는 것이 어디 까지인가?라는 물음이 생겼다 이에 현실적인 환경(자본(장비), 시간... 등)에서 "합격(통과) 기준"을 정해야할 필요성을 가슴 깊게 느낄 수 있었다.
  
- AI에 대한 인식 변화
  > AI를 사용해서 코드를 작성할 때 과연.. "이게 옳은 것일까" 고민을 많이하고 혼자서 구조를 짜서 코드를 작성하고자 시도 해봤고. 배웠던 지식을 활용해서 Moore 방식의 FSM 구조로 코드를 작성하고자 옛날에는 Count 변수를 만들어서 Switch 문에 Count변수를
    이용해 case문으로 제어를 했지만, 그것 보다는 Key에서 이벤트만 보내고 그 이벤트 순서에 맞춰서 일종의 개발자가 구현하고자 하는 동작을 유한한 상태로 만들어 예상 외의 시나리오로 흘러가지 않도록 함의 중요성을 느꼈고 이런 구동 방식의 흐름을 잡아 AI를 통해
    구현 방법을 물어보고 접목해서 검증하는 것이 빠르게 프로젝트를 끝내고 학습 속도도 빠르게 가져갈 수 있음을 느꼈다. 이제는.. AI가 만든 결과물을 보며, "어휴.. 못 믿겠다"는 생각으로는 안되고 AI에 하네스를 잘 세워 내가 원하는 방향으로 흘러가도록 통제하는 것이 필요하고,
    이 결과물을 빠르게 검증해서 아이디어를 실체화하는 것이 더 중요함을 느끼게 되었다. 기술 스택도 중요하지만, 가장 중요한 것은 "구현"이 시작이자 끝이라는 것을 깨닫게 되었다.
  
- 정리의 필요성
  > 이번 프로젝트를 해보면서 GitHUB를 사용하게 되었는데 단순히 개인 프로젝트를 올리고 저장하는 사이트라 생각했었지만, 그게 아니라 git이라는 개념을 조금 알기 시작하게 된 것 같다. 프로젝트를 백업하고, Message를 남겨 누가 무엇을 수정했는지 협업을 할 수 있도록 만든 일종의
    파일 트리 시스템이지만, 생각보다 실용적이고 편의성이 많은 것을 알게 되었고, 그동안 이런저런 많은 경험을 해왔던 것을 git같은 곳에 공개 / 비공개로 나눠 정리 해놨으면, 나의 커리어, 공부 내용 등 유의미한 데이터로 가공할 수 있었을 텐데 싶다. 조금 더 빨리 알았으면 이라는 생각이 들었다.
    "기록"하는 행위가 정말 기초적이고 단순한 행위일 것 같지만, 뭐든 기록해 놓고 "가공"이란 행위를 할 수 있도록 밑 작업을 해야 할 것 같다. 

<details>
<summary><b>😅 주구절절 Note.</b></summary>

### FND Current
- **정격 계산:** FND LED $V_f \approx 2\text{V}$, $I_f \approx 20\text{mA}$ 기준 (8개 세그먼트 전점등 시 총 $160\text{mA}$ 소비). $V_{CC} = 3.3\text{V}$ 공급 시 필요 제한 저항은 $R = \frac{3.3\text{V} - 2.0\text{V}}{0.02\text{A}} = 65\Omega$ ([E96](https://www.electronics-notes.com/articles/electronic_components/resistors/standard-resistor-values-e-series-e3-e6-e12-e24-e48-e96.php) 표준 규격 $64.9\Omega$ 선정).
- **전력 계산:** $P = I^2 R = (0.02\text{A})^2 \times 64.9\Omega \approx 26\text{mW}$이므로 1608 패키지 저항을 세그먼트별로 개별 배치하는 것이 안정적임.
- 회로 단순화를 위해 COM 단자에 $6432$ 저항 1개를 써서 개당 약 $4\text{mA}$ 제어로 구성했으나, 위의 계산식대로하는 것이 정석적으로 보여지기 때문에 추후 사용한다면, 개별적으로 배치함이 타당하다고 판단됨.
  => 실제 구현시 약간 밝기 변화가 있지만, 크게 문제될 정도는 아니었음.

### Buck-Converter
- **LDO:** $5\text{V} \rightarrow 3.3\text{V}$ 변환 시 $P_{loss} = V_{drop} \times I_{load}$에 따른 발열 및 전력 손실을 줄이기 위해 약 90% 효율의 **TLV62569** IC 선택.
- **PG (Power-Good) 핀:** 전원 안정성 모니터링 및 시퀀싱 확인을 위해 설계에 반영.

### USB 2.0
- **Type-C CC 핀:** CC1, CC2 라인에 각각 $5.1\text{k}\Omega$ 풀다운 저항을 배치하여 $V_{BUS}(5\text{V})$ 정상 인가 확보.
- **D+ Pull-Up:** USB Full-Speed 장치 인식을 위해 D+ 라인에 $1.5\text{k}\Omega$ 풀업 저항 배치. 초기에 $V_{BUS}(5\text{V})$ 레벨 연결 오류를 인지하여 STM32 동작 전원인 $3.3\text{V}$ 풀업으로 정정 필요. (추후 디버거 없이 펌웨어를 수정/업로드하기 위한 목적)

</details>
