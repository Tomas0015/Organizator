# Organizator
Jednoduchý systém na organizaci součástek v šuplíkové skříni.
<br />
## Hardware:<br />
mikrokontrolér ESP32, LED pásky kompatibilní s WS2812
## Soubory uložené v ESP:<br />
Všechny soubory leží v rootu. V ESP je vytvořený souborový systém SPIFFS, který obsahuje následující soubory:
<br />
### Konfigurační soubory:<br />
net - obsahuje nastavení sítě, káždá hodnota končí znakem \n, obsah souboru: název AP a mDNS, SSID, heslo<br />
dev - obsahuje nastavení skříní, každá skříňka začíná znakem \r, dále obsahuje pole oddělené znakem \t: číslo datového pinu LED pásku, počet LED na LED pásku, počet LED v řádku, dále pokračuje libovolný počet polí spojených diod (umožňuje používat širší šuplíky)<br />
Příklad:<br />
\rA\t27\t55\t5\t45-47\t48-49\t50-54<br />
<br />
### Databáze<br />
soubor DB obsahuje celou databázi, ta má následující formát:<br />
Znakem \r začíná název kategorie (databáze vždy začíná názvem kategorie).<br />
Znakem \n začíná položka v databázi.<br />
Každá položka má tři pole: název, počet, umístění, pole jsou od sebe odděleny znakem \t<br />
Příklad:<br />
\rRezistory<br />
\n100R 0603 0,125W 1%\t199\tA21<br />
<br />
## Soubory webu<br />
Soubory webu jsou uloženy komprimované pro úsporu místa.<br />
Pro kompresi byl vyžit program 7-zip:<br />
7z a -tgzip {výstupní soubor} {vstupní soubor} -mx=9<br />
<br />
## Implementované API:<br />
/gconf?{název}=<br />
Vrátí soubor uložený v ESP.<br />
<br />
/sconf?{název}={obsah}<br />
Přepíše daný soubor zaslaným obsahem. Při úspěšném zápisu se HTTP odpověď vrátí se statusem 201.<br />
<br />
/sLED?N={index}&P={pin}&R={červená}&G={zelená}&B={modrá}<br />
Rozsvítí jednu LED na LED pásku.<br />
<br />
/sLEDs?S={index první}&L={délka}&P={pin}&R={červená}&G={zelená}&B={modrá}<br />
Rozsvítí rozsah LED na LED pásku.<br />
