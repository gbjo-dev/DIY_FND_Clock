# DIY_FND_Clock 🕙
STM32F103C8T6 공부를 위한 FND 디지털 시계 DIY 프로젝트!

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
   > short 누름은 무시됩니다. ( 추후 다른 페이즈로 넘어가도록 하여 기능 추가 가능.)
2. Enter Time Setting Mode
   > long 누름 입력시 RTC 시간을 읽어와 임시로 set_time에 저장하고 MODE_SET_HOUR 모드로 진입.
   > short 누름시 '시'값을 1씩 올림 원하는 값이 도달시 long 누름 후 빠져 나옴
   > 같은 방식으로 '분'도 설정하고 나옴
3. Apply
   > 마지막 MODE_SET_MIN에서 long 누름 후 빠져나오면 변경된 시간을 RTC register에 갱신 후 MODE_NORMAL로 복귀


<details>
<summary><b>😅 주구절절 Note.</b></summary>

### FND Current
- **정격 계산:** FND LED $V_f \approx 2\text{V}$, $I_f \approx 20\text{mA}$ 기준 (8개 세그먼트 전점등 시 총 $160\text{mA}$ 소비). $V_{CC} = 3.3\text{V}$ 공급 시 필요 제한 저항은 $R = \frac{3.3\text{V} - 2.0\text{V}}{0.02\text{A}} = 65\Omega$ ([E96](https://www.electronics-notes.com/articles/electronic_components/resistors/standard-resistor-values-e-series-e3-e6-e12-e24-e48-e96.php) 표준 규격 $64.9\Omega$ 선정).
- **전력 계산:** $P = I^2 R = (0.02\text{A})^2 \times 64.9\Omega \approx 26\text{mW}$이므로 1608 패키지 저항을 세그먼트별로 개별 배치하는 것이 안정적임.
- 회로 단순화를 위해 COM 단자에 $6432$ 대형 저항 1개를 써서 개당 약 $4\text{mA}$ 제어로 구성했으나, 위의 계산식대로하는 것이 정석적으로 보여지기 때문에 추후 사용한다면, 개별적으로 배치함이 타당하다고 판단됨.

### Buck-Converter
- **LDO:** $5\text{V} \rightarrow 3.3\text{V}$ 변환 시 $P_{loss} = V_{drop} \times I_{load}$에 따른 발열 및 전력 손실을 줄이기 위해 약 90% 효율의 **TLV62569** IC 선택.
- **PG (Power-Good) 핀:** 전원 안정성 모니터링 및 시퀀싱 확인을 위해 설계에 반영.

### USB 2.0
- **Type-C CC 핀:** CC1, CC2 라인에 각각 $5.1\text{k}\Omega$ 풀다운 저항을 배치하여 $V_{BUS}(5\text{V})$ 정상 인가 확보.
- **D+ Pull-Up:** USB Full-Speed 장치 인식을 위해 D+ 라인에 $1.5\text{k}\Omega$ 풀업 저항 배치. 초기에 $V_{BUS}(5\text{V})$ 레벨 연결 오류를 인지하여 STM32 동작 전원인 $3.3\text{V}$ 풀업으로 정정 필요. (추후 디버거 없이 펌웨어를 수정/업로드하기 위한 목적)

</details>
