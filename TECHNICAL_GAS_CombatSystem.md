# Tài liệu kỹ thuật — GAS (CombatSystem)

---

## Mục lục
- [Sơ đồ](#Sơ-đồ)
- [Tài liệu sản phẩm (Product)](#tài-liệu-sản-phẩm-product)
- [Kiến trúc phần mềm (Logical)](#kiến-trúc-phần-mềm-logical)
- [Kiến trúc triển khai (Physical)](#kiến-trúc-triển-khai-physical)
- [Khái niệm GAS trong dự án](#khái-niệm-gas-trong-dự-án)
  - [Gameplay Attribute](#1-gameplay-attribute)
  - [Gameplay Effect (GE)](#2-gameplay-effect-ge)
  - [Gameplay Effect Execution Calculation](#3-gameplay-effect-execution-calculation)
  - [Gameplay Ability](#4-gameplay-ability)
  - [Gameplay Tag](#5-gameplay-tag)
- [Luồng & sequence](#luồng--sequence)
- [Technical decision log](#technical-decision-log)

---

## Sơ đồ

### Class Diagram

![Class Diagram](https://drive.google.com/uc?id=1azAKCG9Et7lbwXpeSL98P6rAKoP8Cckx)

### Game System Diagram

![Game System Diagram](https://drive.google.com/uc?id=1THtrqlyo8IRhUVmoJ0It9SB_gG5SFWtZ)


## Tài liệu sản phẩm (Product)

**Yêu cầu nghiệp vụ (tóm tắt):**

- **Player:** combo làm tăng damage; dodge, perfect dodge, parry; có thể phản đòn khi đúng cửa sổ.
- **Enemy:** nhận damage; thanh **Stun** — đủ ngưỡng → trạng thái choáng / stun lock.
- **Damage:** tính qua GAS (không chỉ trừ float tay); có scale từng đòn và combo (theo spec design).

**Use case (mức cao):**

| Actor | Hành vi chính |
|-------|----------------|
| Player | Đánh → apply GE damage; dodge/parry → GE cấp tag cửa sổ. |
| Enemy | Attack → nếu hit player, kiểm tra tag trên player ASC → kích hoạt Perfect Dodge / Parry. |

---

## Kiến trúc phần mềm (Logical)

**Luồng khái niệm:** mọi nhân vật combat dùng chung một **base character** có **ASC** + **Attribute Set**; damage đi qua **Gameplay Effect** + **Execution Calculation**; phòng thủ dùng **GE duration** + **Gameplay Tag** + **Gameplay Ability**.

```mermaid
flowchart TB
  subgraph pawn [Pawn combat]
    ASC[Ability System Component]
    AS[Attribute Set]
  end
  ASC --> AS
  GE[Gameplay Effect damage] --> ASC
  Exec[Execution Calculation] --> GE
  Ability[Gameplay Ability] --> ASC
```

**Player** và **Enemy** kế thừa cùng base; khác **grant ability**, AI, input.

---

## Kiến trúc triển khai (Physical)

- **Engine:** Unreal Engine 5, module **Gameplay Abilities** (GAS).
- **Code:** C++ cho base character, attribute set, execution damage; Blueprint cho ability cụ thể, montage, asset GE.
- **Không có database** runtime cho combat GAS — trạng thái nằm trên **ASC** + **replication** attribute khi cần multiplayer.

---

## Khái niệm GAS trong dự án

### 1. Gameplay Attribute

Là **số liệu** (thường float) GAS đọc/ghi. Trong dự án dùng một **Attribute Set** chung, gắn ASC.

| Khái niệm | Vai trò trong thiết kế |
|-----------|-------------------------|
| Health / MaxHealth | Nhận damage (trừ máu). |
| Stamina | Tài nguyên cho dodge / skill (GE cost hoặc ability cost). |
| Combo | Chuỗi đánh — chủ yếu **player**, nhân vào damage (theo spec). |
| BaseDamage | Damage gốc mỗi hit — đọc từ attacker khi tính damage. |
| Stun | Thanh posture — **enemy**; đủ ngưỡng → stun. |

---

### 2. Gameplay Effect (GE)

**Hiệu ứng** instant hoặc theo thời gian lên ASC.

**Trong dự án:**

- **GE damage (instant):** dùng **Execution Calculation** để tính một lần — **không** chồng modifier Health trùng kiểu khác (tránh double damage).
- **GE window (duration):** perfect dodge / parry — **Granted Tag** lên player trong vài giây.
- GE khác: cost stamina, buff, v.v.

---

### 3. Gameplay Effect Execution Calculation

**Class tính toán** khi GE có **Execution** — dùng cho damage có combo / scale.

**Trong dự án:** đọc **BaseDamage**, **Combo** từ Attribute Set của **source**; nhân **Scale** từ **SetByCaller** (tag ví dụ `Data.Damage.Scale`); output trừ **Health** của **target**.

---

### 4. Gameplay Ability

**Hành động** có vòng đời: activate → run → end.

**Trong dự án:** grant lên ASC; kích hoạt từ input, AI, Behavior Tree, hoặc **Gameplay Event**.

---

### 5. Gameplay Tag

**Nhãn** phân cấp (`A.B.C`): Required/Blocked ability, Granted tag từ GE, SetByCaller, Event.

**Trong dự án:**

- `Data.Damage.Scale` — SetByCaller cho scale từng đòn.
- `State.PerfectDodgeWindow`, `State.ParryWindow` — counter window.
- `State.Stunned` — enemy (hoặc actor) khi đủ stun.

---

## Luồng & sequence

### Damage (bất kỳ hướng player ↔ enemy)

Build **spec** **GE damage** → apply lên ASC **target** → **Execution Calculation** dùng attribute **source** + **SetByCaller (Scale)** → trừ **Health** target.

### Enemy attack → player (Perfect Dodge / Parry)

**Trước hit:** player dodge/parry → **Gameplay Ability** → **GE duration** → Granted Tag: `State.PerfectDodgeWindow` hoặc `State.ParryWindow` trên ASC player.

**Khi resolve hit (enemy → player):**

1. Kiểm tra ASC player có **`State.PerfectDodgeWindow`** → **Try Activate Ability** **Perfect Dodge** (hoặc trigger tương đương).
2. Nếu cần nhánh parry: kiểm tra **`State.ParryWindow`** → **Try Activate Ability** **Parry** (hoặc ability counter tương ứng).

### Enemy stun

`Stun` tăng khi bị đánh; đủ ngưỡng → trạng thái stun (GE + tag hoặc ability).

### Sơ đồ sequence (mô tả)

```mermaid
sequenceDiagram
  participant E as Enemy
  participant Hit as Hit resolve
  participant P as Player ASC
  Note over P: Trước đó: GE window đã cấp tag trên ASC
  E->>Hit: Attack / hit player
  Hit->>P: Check tags
  alt State.PerfectDodgeWindow
    Hit->>P: Activate Perfect Dodge ability
  else State.ParryWindow
    Hit->>P: Activate Parry ability
  else Không có window
    Hit->>P: Apply GE damage
  end
```

---

## Technical decision log

| Quyết định | Lý do ngắn |
|------------|------------|
| Dùng **GAS** | Quản lý ability, buff/debuff, tag, replication thống nhất. |
| Damage qua **Execution Calculation** | Tránh nhân đôi modifier; gom công thức combo + BaseDamage + Scale một chỗ. |
| **BaseDamage** trên attribute, không truyền tay mỗi hit | Một nguồn sự thật cho damage gốc; buff/debuff qua GE. |
| Cửa sổ phòng thủ bằng **GE + Granted Tag** | Dễ kiểm tra trong hit resolve; tách khỏi damage pipeline nếu cần. |

---
