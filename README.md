Nooa Jutila, 24TietoA

-----

Tässä ohjelmassa on hotellin varausohjelma, suoritettu 5p mukaisesti

Ohjelman UI sekä kommentointi ovat molemmat englanniksi.

Tässä flowchart ohjelman toiminnasta: 

https://imgur.com/3Sho4uy

-----

Ohjelman perustoiminnot:

Arvottu parillinen määrä huoneita väliltä 40-300

Ensimmäiseen puolet huoneista mahtuu yksi, toiseen puoleen kaksi.

100e / yö / 1h huone

150e / yö / 2h huone

Kaikki inputit tarkistettu

Ohjelma kysyy käyttäjältä nimen, ihmisten määrän, öiden määrän ja minkä huonevaihtoehdon haluaa.

-----

Lisätoimintoja:

Ohjelma antaa vaihtoehtoja huoneiden varaamiselle.

Ohjelma antaa myös mahdollisuuden palata taaksepäin lähes kaikissa vaiheissa.

Ohjelmassa on kaksi hakuvaihtoehtoa, nimellä haku tai asiakasnumerolla haku.

-----

Tiedon tallennus:

"RoomInfo.txt" sisältää tiedon huoneiden koosta, varaustilasta ja asiakasnumerosta. Tämä data luetaan 2D vektoriin RoomInfo. Joka neljäs rivi on asiakkaan nimi, joka luetaan eri vektoriin.

Tiedoston syntaksi on:

-----

1 > Huoneen koko, 1 tai 2

0 > Huoneen varauksen tila, 0 vapaa ja 1 varattu

0 > Varausnumero, aluksi 0 kunnes huone varataan, jolloin arvotaan kokonaisluku 10 000 - 99 999

Unreserved > Asiakkaan nimi, aluksi "Unreserved", kunnes huone varataan, jolloin korvaantuu asiakkaan nimellä

-----
