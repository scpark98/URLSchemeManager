# URLSchemeManager

@../Common/claude.md

## 개발 목적

- 입력된 URLScheme string 을 **레지스트리 3곳** 에서 지우거나 3곳 모두 등록할 수 있다.
- 브라우저의 **프로필** (쿠키가 아님) 에 자동 실행 옵션이 등록되어 있다면 찾아서 지워준다.

> 레지스트리 3곳의 정확한 위치와 브라우저 프로필 경로 패턴은 구현 시점에 확정 — 본 메모는 의도만.

## 다이얼로그 컨트롤

- `m_static_urlscheme` (`CSCStaticEdit`, `IDC_STATIC_URLSCHEME`) — URLScheme 문자열 입력.
- `m_combo_browser` (`CSCComboBox`, `IDC_COMBO_BROWSER`) — 시스템 설치 브라우저 목록. 0 번 = 기본 브라우저. `load_browser_list()` 가 채움.
- 버튼: `Register` / `Delete Registry` / `Remove Check`.

## 함수

### `load_browser_list()`

- `HKLM` / `HKCU` 의 `SOFTWARE\Clients\StartMenuInternet` 양쪽 enum 하여 시스템에 등록된 브라우저 친숙 이름 (`Google Chrome`, `Microsoft Edge`, ...) 수집 + 중복 제거.
- 기본 브라우저는 `Common::get_default_browser_info()` (Functions.cpp) 로 획득 — 내부에서 `IApplicationAssociationRegistration::QueryCurrentDefault` 후 실패 시 `HKCU\...\UrlAssociations\https\UserChoice\ProgId` 레지스트리로 폴백.
- 기본 브라우저를 콤보 0 번으로 swap (없으면 insert) 후 `SetCurSel(0)`.

## 인코딩

- `.editorconfig` 로 `.cpp/.h` = UTF-8 BOM, `.rc/.rc2` = UTF-16 LE 강제. VS save 시 자동 fix.
- 새 파일 추가 시 위 규칙 따름.
