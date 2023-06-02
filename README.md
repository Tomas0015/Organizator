# Organizator
Jednoduchý systém na organizaci součástek v šuplíkové skříni.

Hardware:
mikrokontrolér ESP32, LED pásky kompatibilní s WS2812
#Soubory uložené v ESP:
V ESP je vytvořený souborový systém SPIFFS, který obsahuje následující soubory:

Konfigurační soubory:
net - obsahuje nastavení sítě, káždá hodnota končí znakem \n, obsah souboru: název AP a mDNS, SSID, heslo
dev - obsahuje nastavení skříní, každá skříňka začíná znakem \r, dále obsahuje pole oddělené znakem \t: číslo datového pinu LED pásku, počet LED na LED pásku, počet LED v řádku, dále pokračuje libovolný počet polí spojených diod (umožňuje používat širší šuplíky)
#Příklad:
\rA\t27\t55\t5\t45-47\t48-49\t50-54

Databáze
soubor DB obsahuje celou databázi, ta má následující formát:
Znakem \r začíná název kategorie (databáze vždy začíná názvem kategorie).
Znakem \n začíná položka v databázi.
Každá položka má tři pole: název, počet, umístění, pole jsou od sebe odděleny znakem \t
Příklad:
\rRezistory
\n100R 0603 0,125W 1%\t199\tA21

Soubory webu
Soubory webu jsou uloženy komprimované pro úsporu místa.
Pro kompresi byl vyžit program 7-zip:
7z a -tgzip {výstupní soubor} {vstupní soubor} -mx=9

Implementované API:
/gconf?{název}= 
Vrátí soubor uložený v ESP.

/sconf?{název}={obsah}
Přepíše daný soubor zaslaným obsahem. Při úspěšném zápisu se HTTP odpověď vrátí se statusem 201.

/sLED?N={index}&P={pin}&R={červená}&G={zelená}&B={modrá}
Rozsvítí jednu LED na LED pásku.

/sLEDs?S={index první}&L={délka}&P={pin}&R={červená}&G={zelená}&B={modrá}
Rozsvítí rozsah LED na LED pásku.
