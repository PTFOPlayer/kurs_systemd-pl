# Podstawowe wiadomości
## Czym jest systemd
Można myśleć o nim jak o starszym init, czyli o programie który ma na celu uruchomienie jakiegoś procesu przy auto-starcie. Systemd zaczyna swoje działanie w PID 1 ( inicjalizacja systemu ) gdzie najpierw uruchamia wszystkie podstawowe procesy konieczne do poprawnego uruchomienia systemu, a następnie procesy związane z odpowiadającymi poziomami SysV dbając o kolejność uruchamiania tych procesów. Systemd udostępnia urzytkownikowi równierz opcję tworzenia wielowątkowości w programach poprzez udostępnianie urzytkownikowi socketów i D-bus (komunikacja między procesowa). 

### Czym jest Daemon?
Daemon (a w nazewnictwie systemd "service" czyli serwis oraz "unit" czyli segment) jest po prostu zestawem zasad do uruchmoeniania danego procesu pracującego w tle, do którego urzytkownik nie ma dostepu. Na przykład syslogd jest daeomonem odpowiadającym za logowanie urzytkownika, sam użytkownik nie ma dostępu do samego procesu, ale używa go pośrednio podczas logowania się do systemu

### Przykłady procesów
Do przykładowych procesów wywoływanych przez systemd możemy zaliczyć: wyłączanie i ponownym uruchomienie systemu, montowanie urządzeń pamięci masowej, uruchamianie menedżera logowania, procesy konfiguracji systemu (ex. nazwa hosta jest ustawiana przez systemd przy uruchomieniu), synchronizacja czasu i lokalizacji, maszyny wirtualne i kontenery.

# Użytkowanie
## Komendy zwracające stan systemu
| komenda                                           | akcja                                     | uwagi                                                              |
|---------------------------------------------------|-------------------------------------------|--------------------------------------------------------------------|
| systemctl status                                  | podaje status wszystkich daemonów         |                                                                    |
| systemctl                                         | zwraca listę aktywnych daeomonów          | z flagą `--failed` zwraca daemony które nie poprawnie wystartowały |
| systemctl list-unit-files                         | zwraca listę zainstalowanych daemonów     |                                                                    |
| systemctl status `<nazwa_unita/nazwa_daemona>`    | zwraca status danego daemona              | informuje równierz czy dany unit jest uruchomiony                  | 
| systemctl help `<nazwa_unita/nazwa_daemona>`      | zwraca man-page dla danego unita          | działa tylko jeżeli dany unit ma swój man-page                     |
| systemctl is-enabled `<nazwa_unita/nazwa_daemona>`| mówi czy dany unit jest wrzucony do autostartu |                                                               |
>Notka: powyższe komendy (zazwyczaj) nie muszą być wykonywane jako root

## Zarządzanie serwisami i unitami

| komenda                                           | akcja                                     | uwagi                                                        |
|---------------------------------------------------|-------------------------------------------|--------------------------------------------------------------|
| systemctl start `<nazwa_unita/nazwa_daemona>`     | rozpoczyna pracę danego unitu             |                                                              |
| systemctl stop `<nazwa_unita/nazwa_daemona>`      | kończy pracę danego unitu                 |                                                              |
| systemctl restart `<nazwa_unita/nazwa_daemona>`   | restartuje dany unit                      |                                                              |
| systemctl reload `<nazwa_unita/nazwa_daemona>`    | restartuje dany unit i jego konfiguracje  |                                                              |
| systemctl daemon-reload                           | reload ale dla wszystkich serwisów        |                                                              |
| systemctl enable `<nazwa_unita/nazwa_daemona>`    | wrzuca dany unit do autostartu            | z flagą `--now` wykonuje dodatkowo to samo co start          |
| systemctl disable `<nazwa_unita/nazwa_daemona>`   | usuwa dany unit z autostartu              |                                                              |
| systemctl enable `<nazwa_unita/nazwa_daemona>`    | dodaje dany unit do autostartu            |                                                              |
| systemctl reenable `<nazwa_unita/nazwa_daemona>`  | przywraca dany unit do autostartu         | jeżeli unit jest w autostarcie to go usuwa i dodaje ponownie |
>Notka: wszystkie te komendy powinny być wykonane jako root

## Zarządzanie zasilaniem za pomocą systemctl

| komenda               | akcja                         |
|-----------------------|-------------------------------|
| systemctl reboot      | uruchamia ponownie urządzenie |
| systemctl power-off   | wyłącza urządzenie            |
| systemctl suspend     | zawiesza pracę systemu        |
| systemctl hibernate   | usypia urządzenie             |
| systemctl hybrid-sleep| połączenie suspend i hibernate|

# Pisanie własnych serwisów

## Określanie podstawowych informacji o unicie odbywa się w sekcji Unit:
`[Unit]`

## W sekcji unit mogą się znajdować takie właściwości jak:
* `Description` - odpowiada za opis unitu
* `Name` - odpowiada za nazwe
* `Requires` - określa jaki inny unit jest potrzebny do działania tego
* `After` - określa po wystartowaniu jakiego unita ten ma się włączyć
* `Wants` - działa podobnie do `Requires` lecz jeśli unit nie istnieje to nie będzie oczekiwał na jego uruchomienie 
* `OnFailure` - określa co się ma wydażyć po błędzie

## Określenie wykonywanych akcji przez dany unit odbywa się w sekcji Service:
`[Service]`
* `ExecStart` - rozpoczyna proces
* `ExecStop` - kończy proces
* `Restart` - określa kiedy proces powinien być uruchomiony ponownie
* `RestartSec` - określa po ilu sekundach proces zostanie uruchomiony ponownie (musi być określone kiedy proces ma być restartowany)
* `Type` - określa typ serwicsu

### Dostępne typy:

* `Type = simple` - ustawiany domyślnie, umożliwia odpalanie procesu jedno wątkowego ( wielowątkowy zadziała ale systemd nie będzie śledził podprocesów) 
* `Type = forking` - umożliwia odpalenie procesu który działa na wielu wątkach i systemd śledzi podprocesy (zlaeca się dodanie `PIDFile=<ścieżka_do_pliku>` w celu zapewnienia prawidłowego śledzenia procesów)
* `Type = oneshot` - systemd uruchamia proces który ma się zakończyć z kodem po wykonaniu zadania (dodanie dodatkowo `RemainAfterExit=Yes` powoduje że po zakończeniu procesu systemd dalej uznaje serwis za aktywny)
* `Type = notify` - to samo co simple lecz systemd dostanie informacje co do momentu gotowości procesu (gotowość nie znaczy że proces się rozpocznie)
* `Type = dbus` - serwis zostaje uznany za gotowy do odpalenia jeżeli `BusName` pojawi się w dbus
* `Type = idle` - to samo co simple lecz systemd zaczeka aż wszystkie procesy będą możliwe do odpalenia jednocześnie

## Określanie momentu odpalenia serwisu następuje w sekcji Install:
`[Install]`
* `WantedBy` - określa przed wejściem w jaki `runlevel` serwis ma być odpalony

| Runlevel | cel                                                    | uwagi                                                             |
|----------|--------------------------------------------------------|-------------------------------------------------------------------|
|0         | runlevel0.target, poweroff.target                      | działa tuż przed wyłączeniem systemu                              |
|1, s      | runlevel1.target, singleuser.target                    | tryb pojedyńczego urzytkownika                                    |
|2, 4      | runlevel2.target, runlevel2.target, multi-user.target  | zdefiniowane przez użytkowinka, domyślnie to samo co poziom 3     |
|3         | runlevel3.target, multi-user.target                    | wiele urzytkowników bez trybu graficznego (wiele konsol)          |
|5         | runlevel5.target, graphical.target                     | tryb graficzny                                                    |
|6         | runlevel6.target, reboot.target                        | działa tuż przed rebootem                                         |
|emergency | emergency.target                                       | shell awaryjny                                                    |

>Notka: Jeżeli w `[unit]` jest określony `Requires=` z jakimś runlevel-em to serwis będzie uruchamiany po tym runlevelu a nie w trakcie uruchamiania

## Dodawanie unitów do systemd
Jest wiele miejsc w których znajdują się odpalalne serwisy systemd, między innymi:
 * `/lib/systemd/system` - pliki dodane przez distro
 * `/usr/lib/systemd/system` - pliki dodane przez pakiety
 * `/run/systemd/system` - tymczasowe 
 * `/etc/systemd/system` - to nas interesuje

`/etc/systemd/system` posiada pliki skonfigurowane przez użytownika, ma najwyższy proirytet więc jeśli znajduje się gdzieś indziej unit o takiej samej nazwie jak w tej lokalizacji to nie zostanie od uruchomiony

>Notka: dobrą praktyką jest wykonanie `systemctl daemon-reload` po dodaniu nowego unita żeby systemd sprawdził sobie na nowo zależności pomiędzy unitami 