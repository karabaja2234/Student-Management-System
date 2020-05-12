#include <cstdlib>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <cstring>
#include <string.h>
#include <windows.h>
#include <sstream>
#include <stdio.h>
#include <fstream>

using namespace std;

char * crt = "\n\t\t=========================================================\n";

//Enumeracija za godinu studija studenata
enum godinaStudija {
	prva = 1, druga, treca, cetvrta
};

//Enumeracija za nacin studiranja studenata
enum nacinStudiranja {
	redovan, vanredni
};

//Enumeracija za odgovore studenata
enum Odgovori {
	NETACNO, TACNO
};

//Enumeracija za razlikovanje admina i studenata na osnovu uloge u sistemu
enum korisnickaUloga {
	student, administrator
};

//Struktura za svaku osobu koja se registruje u sistemu
//Sadrzi samo najvaznije informacije po kojima je osoba prepoznatljiva
struct Osoba {
	string ime, prezime, username, password;	
};

//Struktura za admina: Dodana varijabla "admin" na osnovu strukture "Osoba" kako bi se admin razlikovao od studenata
//Uloga admina je po defaultu postavljena na "administrator"
struct Administrator {
	Osoba administrator;
	int specijalniID;
	korisnickaUloga uloga;
};

//Struktura za studenta: Dodana varijabla "student" na osnovu strukture "Osoba" kako bi se student razlikovao od admina
//Uloga studenta je po defaultu postavljena na "student"
struct Student {
	Osoba student;
	int brIndeksa;
	korisnickaUloga uloga;
	bool radioTest;
	nacinStudiranja nacin;
	godinaStudija godina;
	Odgovori odgovori[100];
};

//Struktura za pitanja
struct Pitanje {
	char tekstPitanja[100];
	Odgovori odgovorNaPitanje;
};

//Stuktura za datum
struct datum{
	int dan, mjesec, godina;
};

//Funkcija za ispis datuma
void info(datum date){
	cout << setfill('0') << setw(2)
	     << date.dan << "." << setfill('0')
	     << setw(2) << date.mjesec << "."
	     << date.godina << ".";
}

//Funkcija za provjeru ispravnosti datuma
bool provjera(int dan, int mjesec, int godina) {
	if (dan > 0 && mjesec > 0 && mjesec < 13 && godina > 0) {		//Prvi uslov provjerava da li su je dan, mjesec i godina pozitivni brojevi, ukljucujuci i uslov da je mjesec manji od 13
	//Provjera za prestupne godine (mjesec februar mora biti manji od 30)
      if (godina % 4 == 0 && godina > 0 && mjesec > 0 && mjesec < 13 && dan > 0) {
        if (mjesec == 2) {
          if (dan > 0 && dan < 30) {
            return true;
          } else return false;
        } else if (mjesec == 1 || mjesec == 3 || mjesec == 5 || mjesec == 7 || mjesec == 8 || mjesec == 10 || mjesec == 12) {
          if (dan > 0 && dan < 32) {
            return true;
          } else return false;
        } else {
          if (dan > 0 && dan < 31) {
            return true;
          } else return false;
        }
        //Provjera za godine koje nisu prestupne (mjesec februar mora biti manji od 29)
      } else {
        if (mjesec == 2) {
          if (dan > 0 && dan < 29) {
            return true;
          } else return false;
        } else if (mjesec == 1 || mjesec == 3 || mjesec == 5 || mjesec == 7 || mjesec == 8 || mjesec == 10 || mjesec == 12) {
          if (dan > 0 && dan < 32) {
            return true;
          } else return false;
        } else {
          if (dan > 0 && dan < 31) {
            return true;
          } else return false;
        }
      }
    } else return false;
    //Funkcija vraca 1 ako je datum ispravan, a 0 ako nije
}

//Globalne varijable za broj maksimalni broj pitanja, maksimalni broj studenata, te trenutni broj pitanja i studenata
const int maxPitanja = 100;
const int maxStudenata = 100;
int brojPitanja = 0;
int brojStudenata = 0;

//Globalne varijable za niz pitanja, niz studenata i admina (koji je u ovom slucaju samo 1)
Pitanje pitanjaDB[maxPitanja];
Student nizStudenata[maxStudenata];
Administrator admin;

//Funkcija koja dodaje dodatno pitanje: prosljedjuje joj se tekst pitanja i odgovor na osnovu enumeracije "Odgovori"
void Dodaj(char *tekstPitanja, Odgovori tacanOdgovor) {
	Pitanje nekoPitanje;
	strcpy(nekoPitanje.tekstPitanja, tekstPitanja);
	nekoPitanje.odgovorNaPitanje = tacanOdgovor;
	pitanjaDB[brojPitanja] = nekoPitanje;
	brojPitanja++;
}

//Funkcija za dodavanje defaultnih pitanja za test
void DodajDefaultPitanja() {
	Dodaj("Enumeracije i unije su sinonimi?", NETACNO);
	Dodaj("Moguce je kreirati anonimnu uniju?", TACNO);
	Dodaj("Svi clanovi unije koriste istu memorijsku lokaciju?", TACNO);
	Dodaj("Svi clanovi strukture koriste istu memorijsku lokaciju?", NETACNO);
	Dodaj("Unija moze biti deklarisana unutar strukture?", TACNO);
}

//Funkcija koja ispisuje pitanje na osnovu proslijedjenog rednog broja tog pitanja
void prikaziPitanje(int rbPitanja) {
	cout << "\n\t\t\t" << pitanjaDB[rbPitanja].tekstPitanja << endl;
}

//Funkcija na osnovu enumeracije "Odgovori" koja vraca odgovor na pitanje (0/1)
Odgovori dajTacanOdgovor (int rbPitanja) {
	return pitanjaDB[rbPitanja].odgovorNaPitanje;
}

//Funkcija koja vraca broj tacno odgovorenih pitanja na testu za studenta koji joj je proslijedjen
int dajBrojTacnih(Student &s) {
	int brTacnih = 0;
	for (int i=1; i<=brojPitanja; i++) {
		if(s.odgovori[i] == dajTacanOdgovor(i)) {
			brTacnih++;
		}
	}
	return brTacnih;
}

//Funkcija za unos podataka: imePrezime, username, password, nacinStudiranja, godinaStudija
//Vrijednosti korisnickaUsluga i radioTest su po defaultu: student i false (respektivno)
//Ova funkcija ujedno provjerava i username-e svih studenata, kao i brojeve indexa, koji se ne smiju ponavljati ni za jednog usera
//Nijedan student ne smije imati ni username admina, a username admina je: selman (navedeno u glavnom programu)
void unosPodataka() {
	system("cls");
	
	//Ukoliko je broj studenata 0, tj. ako se unosi prvi student, za njega se provjerava samo unos username-a, koji ne smije biti isti kao adminov
	if(brojStudenata==0) {
		cout << "\n\n\n\n";
		cout << crt << "\t\t\t\t::Register::" << crt;
		cout << "\t\tRegistracija za " << brojStudenata+1 << ". studenta:\n";
		//Unos imena
		cout << "\t\t\tUnesite ime studenta: ";
		getline(cin, nizStudenata[brojStudenata].student.ime);
		cout << "\t\t\tUnesite prezime: ";
		getline(cin, nizStudenata[brojStudenata].student.prezime);
		
		//Petlja za unos username-a, koja radi sve dok se ne unese username razlicit od adminovog
		do {
			cout << "\t\t\tUnesite username: ";
			getline(cin, nizStudenata[brojStudenata].student.username);
			if(nizStudenata[brojStudenata].student.username == admin.administrator.username) {
				cout << "\t\t\tGreska: Uneseni username je vec u upotrebi\n";
			} else break;	
		} while(1);
		
		//Unos passworda (moze biti bilo koji)
		cout << "\t\t\tUnesite password: ";
		getline(cin, nizStudenata[brojStudenata].student.password);
		
		//Do while petlja koja radi sve dok se index ne unese kao broj
		do {
			cout << "\t\t\tUnesite broj indexa: ";
			cin >> nizStudenata[brojStudenata].brIndeksa;
			if(!cin) {
				cin.clear();
    			cout << "\t\t\tGreska: Uneseni index mora biti broj\n";
    			cin.ignore(10000, '\n');
			} else break;
			break;
		} while(1);
		
		//Unos nacina studiranja
		int x;
		do {
			cout << "\t\t\tNacin studiranja (0 - redovan, 1 - vanredan): ";
			cin >> x;
		} while(x<0 || x>1);
		nizStudenata[brojStudenata].nacin = (nacinStudiranja)x;
		
		//Unos godine studija
		int y;
		do {
			cout << "\t\t\tUnesite godinu studija (1,2,3 ili 4): ";
			cin >> y;
		} while(y<1 || y>4);
		
		//Dodjeljivanje vrijednosti
		nizStudenata[brojStudenata].godina = (godinaStudija)y;
		nizStudenata[brojStudenata].radioTest = false;
		nizStudenata[brojStudenata].uloga = (korisnickaUloga)student;
		
		//Dodavanje studenta u bazu podataka
		ofstream dodajStudenta("bazaPodataka.txt", ios::app);
		dodajStudenta << nizStudenata[brojStudenata].student.ime << " ";
		dodajStudenta << nizStudenata[brojStudenata].student.prezime << " ";
		dodajStudenta << nizStudenata[brojStudenata].student.username << " ";
		dodajStudenta << nizStudenata[brojStudenata].student.password << " ";
		dodajStudenta << nizStudenata[brojStudenata].brIndeksa << " ";
		dodajStudenta << nizStudenata[brojStudenata].radioTest << " ";
		dodajStudenta << nizStudenata[brojStudenata].nacin << " ";
		dodajStudenta << nizStudenata[brojStudenata].godina << endl;
		
		brojStudenata++;
		
	} else if (brojStudenata>0) {	//Ukoliko je broj studenata veci od 0, znaci da moramo za sve takve studente provjeravati i username i broj indexa, 
									//jer nijedan od njih ne smije imati ni isti username kao prethodno uneseni studenti/admin, kao ni broj indexa
		//Unos informacija
		cout << "\n\n\n\n";
		cout << crt << "\t\t\t\t::Register::" << crt;
		cout << "\t\tRegistracija za " << brojStudenata+1 << ". studenta:\n";
		cout << "\t\t\tUnesite ime studenta: ";
		getline(cin, nizStudenata[brojStudenata].student.ime);
		cout << "\t\t\tUnesite prezime: ";
		getline(cin, nizStudenata[brojStudenata].student.prezime);
		
		//Petlja za provjeru validnosti unesenog username-a
		do {
			bool validan = false;	//Vrijednost za provjeru validnosti username-a
			
			//do while petlja koja radi sve dok se ne pronadje username razlicit od svih unesenih do tog trenutka, ukljucujuci i username admina
			do {
				cout << "\t\t\tUnesite username: ";
				getline(cin, nizStudenata[brojStudenata].student.username);
				for(int i=0; i<brojStudenata; i++) {
					if(nizStudenata[brojStudenata].student.username == nizStudenata[i].student.username || nizStudenata[brojStudenata].student.username == admin.administrator.username) {
						cout << "\t\t\tGreska: Uneseni username je vec u upotrebi\n";
						validan = false;
						break;
					} else validan=true;	
				}	
			} while(validan==false);
			break;
		} while(1);
		
		//Unos passworda (moze biti bilo koji)
		cout << "\t\t\tUnesite password: ";
		getline(cin, nizStudenata[brojStudenata].student.password);
		
		//do while petlja koja radi sve dok se ne unese broj indexa koji je broj i koji je razlicit od svih prethodno unesenih indexa
		do {
			bool validan = false;	//Vrijednost za provjeru validnosti indexa
			
			//do while petlja koja radi sve dok se ne pronadje indeks razlicit od svih unesenih do tog trenutka
			do {
				cout << "\t\t\tUnesite broj indexa: ";
				cin >> nizStudenata[brojStudenata].brIndeksa;
				for(int i=0; i<brojStudenata; i++) {
					if(nizStudenata[brojStudenata].brIndeksa == nizStudenata[i].brIndeksa) {
						cout << "\t\t\tGreska: Uneseni broj indexa je vec u upotrebi\n";
						validan = false;
						break;
					} else validan=true;	
				}	
			} while(validan==false);
			
			//Provjera validnosti broja
			if (!cin) {
				cin.clear();
    			cout << "\t\t\tGreska: Uneseni index mora biti broj\n";
    			cin.ignore(10000, '\n');
			} else break;	
		} while(1);
		
		//Unos nacina studiranja
		int x;
		do {
			cout << "\t\t\tNacin studiranja (0 - redovan, 1 - vanredan): ";
			cin >> x;
		} while(x<0 || x>1);
		nizStudenata[brojStudenata].nacin = (nacinStudiranja)x;
		
		//Unos godine studija
		int y;
		do {
			cout << "\t\t\tUnesite godinu studija (1,2,3 ili 4): ";
			cin >> y;
		} while(y<1 || y>4);
		
		//Dodjeljivanje vrijednosti
		nizStudenata[brojStudenata].godina = (godinaStudija)y;
		nizStudenata[brojStudenata].radioTest = false;
		nizStudenata[brojStudenata].uloga = (korisnickaUloga)student;
		
		//Dodavanje studenta u bazu podataka
		ofstream dodajStudenta("bazaPodataka.txt", ios::app);
		dodajStudenta << nizStudenata[brojStudenata].student.ime << " ";
		dodajStudenta << nizStudenata[brojStudenata].student.prezime << " ";
		dodajStudenta << nizStudenata[brojStudenata].student.username << " ";
		dodajStudenta << nizStudenata[brojStudenata].student.password << " ";
		dodajStudenta << nizStudenata[brojStudenata].brIndeksa << " ";
		dodajStudenta << nizStudenata[brojStudenata].radioTest << " ";
		dodajStudenta << nizStudenata[brojStudenata].nacin << " ";
		dodajStudenta << nizStudenata[brojStudenata].godina << endl;
		
		brojStudenata++;	
	}
}

//Meni koji se pojavljuje samo ako sistem prepozna da se admin ulogovao
int adminMeni() {
	int izbor;
	cout << "\n\n\n\n";
	do {
		cout << crt << "\t\t\t::ADMIN " << admin.administrator.ime << " " << admin.administrator.prezime << "::" << crt;
		cout << "\t\t1. Unos pitanja" << endl;
		cout << "\t\t2. Unos novog studenta" << endl;
		cout << "\t\t3. Prikazi rezultate testova" << endl;
		cout << "\t\t4. Pretraga" << endl;
		cout << "\t\t5. Resetuj test za studenta" << endl;
		cout << "\t\t6. Prikazi sve studente" << endl;
		cout << "\t\t7. Prikazi sva pitanja sa testa" << endl;
		cout << "\t\t8. Evidentiraj sve informacije o registrovanim studentima" << endl;
		cout << "\t\t9. Kraj rada" << crt;
		cout << "\t\tUnesite izbor: ";
		cin >> izbor;
	} while (izbor<1 || izbor>9);
	return izbor;
}

//Meni koji se pojavljuje samo ako sistem prepozna da se jedan od studenata ulogovao
int studentMeni() {
	system("cls");
	int izbor;
	cout << "\n\n\n\n";
	do {
		cout << crt << "\t\t\t::STUDENT MENI::" << crt;
		cout << "\t\t1. Pokreni test" << endl;
		cout << "\t\t2. Prikazi rezultate" << endl;
		cout << "\t\t3. Kraj rada" << crt;
		cout << "\t\tUnesite izbor: ";
		cin >> izbor;
	} while (izbor<1 || izbor>3);
	return izbor;
}

//Funkcija koja je zaduzena za vodjenje evidencije o podacima studenata
//Ova funkcija se poziva po zelji adminitratora, upisuje se datum evidencije, te se svi podaci aktuelni u tom momenti pohranjuju u datoteku "evidencija.txt"
void evidencija() {
	datum datumEvidencije;	//Datum evidencije
	
	//Unos datuma evidentiranja podataka za studente
	do {
		cout << "\t\tUnesite datum evidentiranja (d,m,g): ";
		cin >> datumEvidencije.dan >> datumEvidencije.mjesec >> datumEvidencije.godina;
		if(!provjera(datumEvidencije.dan, datumEvidencije.mjesec, datumEvidencije.godina)) {	//Ukoliko datum nije validan, petlja ce ispisati error
			cout << "\t\t\tUneseni datum je neispravan!\n";
		}		
	} while(!provjera(datumEvidencije.dan, datumEvidencije.mjesec, datumEvidencije.godina));	//Petlja ce raditi sve dok datum nije validan, jer ocekuje da je on validan
	
	cout << "\n\t\t="; Sleep(100); cout << "="; Sleep(100); cout << "="; Sleep(100); cout << "=";
				Sleep(100); cout << "="; Sleep(100); cout << " Podaci uspjesno evidentirani!\n";
				Sleep(1500);
	
	//Pravimo ofstream fajl "ispisiPodatke" preko kojeg ce se ispisivati svi trenutni podaci o studentima u text file "evidencija.txt"
	//Ovo je fajl u kojem se podaci appendaju jedan na drugi, jer je potrebno da se vodi evidencija o studentima, testovima itd.
	ofstream ispisiPodatke("evidencija.txt", ios::app);
	if(brojStudenata < 1) {
		cout << "\n\n\tTrenutno nema registrovanih studenata za evidentirati!\n";
	} else {
		for (int i=0; i<brojStudenata; i++) {
			ispisiPodatke << "Ime i prezime studenta: " << nizStudenata[i].student.ime << " " << nizStudenata[i].student.prezime << endl;
			ispisiPodatke << "Broj indeksa studenta: " << nizStudenata[i].brIndeksa << endl;
			if(nizStudenata[i].nacin == 0) {
				ispisiPodatke << "Status studenta: REDOVAN" << endl;
			} else ispisiPodatke << "Status studenta: VANREDAN" << endl;
			ispisiPodatke << "Godina studija: " << nizStudenata[i].godina << "." << endl;
			if(nizStudenata[i].radioTest) ispisiPodatke << "Radio/la test: DA" << endl;
			else ispisiPodatke << "Radio/la test: NE" << endl;
			ispisiPodatke << "Podaci o studenti evidentirani: ";
			ispisiPodatke << setfill('0') << setw(2)
	     	<< datumEvidencije.dan << "." << setfill('0')
	     	<< setw(2) << datumEvidencije.mjesec << "."
	     	<< datumEvidencije.godina << ".";
	     	ispisiPodatke << "\n\n";
		}
	}
	ispisiPodatke.close();
}

//Funkcija koja pri pokretanju programa automatski ucitava podatke o studentima iz fajla "bazaPodataka.txt"
//Ako korisnik zeli dodavati studente direktno kroz datoteku potrebno je da unosi podatke u sljedecem formatu:
//	imeStudenta prezimeStudenta username password brojIndeksa radioTest nacinStudiranja godinaStudija
void ucitajStudente() {
	int godina, nacin;
	ifstream ucitaj;
	ucitaj.open("bazaPodataka.txt");
	while(ucitaj >> nizStudenata[brojStudenata].student.ime >> nizStudenata[brojStudenata].student.prezime >> nizStudenata[brojStudenata].student.username >> nizStudenata[brojStudenata].student.password >> nizStudenata[brojStudenata].brIndeksa >> nizStudenata[brojStudenata].radioTest >> nacin >> godina) {
		nizStudenata[brojStudenata].nacin = (nacinStudiranja)nacin;
		nizStudenata[brojStudenata].godina = (godinaStudija)godina;
		nizStudenata[brojStudenata].uloga = (korisnickaUloga)student;
		brojStudenata++;
	}
	ucitaj.close();
}

//Funkcija koja azurira podatke o studentima u bazi podataka
//Ova funkcija se automatski poziva nakon svakog uradjenog testa, nakon resetovanja testa za nekog od studenta ili za sve studente
void azurirajPodatke() {
	ofstream azuriraj("bazaPodataka.txt", std::ofstream::trunc);
	for (int i=0; i<brojStudenata; i++) {
		azuriraj << nizStudenata[i].student.ime << " ";
		azuriraj << nizStudenata[i].student.prezime << " ";
		azuriraj << nizStudenata[i].student.username << " ";
		azuriraj << nizStudenata[i].student.password << " ";
		azuriraj << nizStudenata[i].brIndeksa << " ";
		azuriraj << nizStudenata[i].radioTest << " ";
		azuriraj << nizStudenata[i].nacin << " ";
		azuriraj << nizStudenata[i].godina << " ";
		azuriraj << endl;
	}
	azuriraj.close();
}


//Funkcija za odabir prikazanih studenata
int prikaziStudente () {
	if (brojStudenata == 0) {
		cout << "\t\tTrenutno nema registrovanih studenata!\n";
		Sleep(2000);
		system("cls");
	} else {
		int rb;
		cout << setw(3) << "\n\t\tRb. " << "Ime i prezime\n";
		for (int i=0; i<brojStudenata; i++) {
			cout << "\t\t" << i+1 << ". " << nizStudenata[i].student.ime << " " << nizStudenata[i].student.prezime << endl;
		}
		do {
			cout << "\t\tUnesite redni broj studenta: ";
			cin >> rb;
		} while(rb<1 || rb>brojStudenata);
		return rb-1;
	}
}

//Funkcija za prikaz popisa imena i prezimena svih studenata
void prikaz() {
	if (brojStudenata == 0) {
		cout << "\t\tTrenutno nema registrovanih studenata!\n";
		Sleep(1500);
		system("cls");
	} else {
		cout << setw(3) << "\n\t\tRb. " << "Ime i prezime\n";
		for (int i=0; i<brojStudenata; i++) {
			cout << "\t\t" << i+1 << ". " << nizStudenata[i].student.ime << " " << nizStudenata[i].student.prezime << endl;
		}
	}	
}

//Funkcija za prikaz svih pitanja
void prikazPitanja() {
	cout << setw(3) << "\n\t\tPitanja za test: \n";
		for (int i=0; i<brojPitanja; i++) {
			cout << "\t\t" << i+1 << ". " << pitanjaDB[i].tekstPitanja << endl;
	}
	cout << endl;
}

//Funkcija za unos dodatnih pitanja
void unosPitanja() {
	cin.ignore();
	cout << "\t\tUnesite tekst pitanja: ";
	cin.getline(pitanjaDB[brojPitanja].tekstPitanja, 100);
	cout << "\t\tUnesite odgovor na pitanje (0 - NETACNO, 1 - TACNO)\n";
	cout << "\t\tOdabir: ";
	int x;
	cin >> x;
	pitanjaDB[brojPitanja].odgovorNaPitanje = (Odgovori)x;
	brojPitanja++;
}

//Funkcija za pretragu registrovanih studenata na osnovu odredjenih definisanih uslova
void pretraga() {
	system("cls");
	if (brojStudenata == 0) {
		cout << "\t\tTrenutno nema registrovanih studenata!\n";
		Sleep(1500);
		system("cls");
	} else {
		int izbor;
		cout << "\n\n\n\n";
		do {
			cout << crt << "\t\t\t\t::PRETRAGA MENI::" << crt;
			cout << "\t\t\t1. Po godini studija" << endl;
			cout << "\t\t\t2. Po nacinu studiranja" << endl;
			cout << "\t\t\t3. Po rezultatima testa" << endl;
			cout << "\n\t\t\tUnesite izbor: ";
			cin >> izbor;
		} while (izbor<1 || izbor>3);
	
		if (izbor == 1) {
			int g;
			cout << "\n\t\tUnesite godinu studija za ispis studenata: ";
			cin >> g;
			int brojac = 1;
				for (int i=0; i<brojStudenata; i++) {
					if(nizStudenata[i].godina == (godinaStudija)g) {
						cout << "\t\t" << brojac++ << ". " << nizStudenata[i].student.ime << " " << nizStudenata[i].student.prezime << endl;
					}
				}
			if(brojac==1) {
				cout << "\t\tNema studenata na toj godini! \n";
			}
		} else if (izbor == 2) {
			cout << "\n\t\tUnesite nacin studiranja (0 - redovan, 1 - vanredni): ";
			int n; cin >> n;
			int brojac = 1;
			for (int i=0; i<brojStudenata; i++) {
				if(nizStudenata[i].nacin == (nacinStudiranja)n) {
					cout << "\t\t" << brojac++ << ". " << nizStudenata[i].student.ime << " " << nizStudenata[i].student.prezime << endl;
				}
			}
			if(brojac==1) {
				cout << "\t\tNema studenata sa tim statusom! \n";
			}
		} else if (izbor == 3) {
			int minP, maxP;
			int brojac = 1;
			cout << "\n\t\tUnesite minimalni postotak: "; cin >> minP;
			cout << "\t\tUnesite maksimalni postotak: "; cin >> maxP;
				for (int i=0; i<brojStudenata; i++) {
					int t = dajBrojTacnih(nizStudenata[i]);
					int p = 100*t/brojPitanja;
					if (p>minP && p<maxP) {
						cout << "\t\t" << brojac++ << ". " << nizStudenata[i].student.ime << " " << nizStudenata[i].student.prezime << endl;
					}
				}	
			if(brojac==1) {
				cout << "\t\tNema studenata u tom rasponu prosjeka! \n";
			}
		}
	}	
} 

//Meni koji se pojavljuje pri pokretanju sistema (3 opcije: Login, Register ili Exit)
int loginMeni() {
	int izbor;
	cout << "\n\n\n\n";
	do {
		cout << crt << "\t\t\t\t::Login / Register::" << crt;
		cout << "\t\t\t\t1. Login" << endl;
		cout << "\t\t\t\t2. Registrujte novog usera" << endl;
		cout << "\t\t\t\t3. Napustite sistem" << endl;
		cout << "\n\t\t\t\tUnesite izbor: ";
		cin >> izbor;
		cin.ignore();
	} while (izbor<1 || izbor>3);
	return izbor;
}

//Funkcija koja unosi tekst za prosljedjene varijable username-a i passworda, te vraca odredjene vrijednosti na osnovu unesenih podataka
int login(string username, string password) {
	//Unos username-a i passworda
	cout << "\n\n\n\n";
	cout << crt << "\t\t\t\t\t::Login::" << crt;
	cout << "\t\t\t\tUsername: ";
	getline(cin, username);
	cout << "\t\t\t\tPassword: ";
	getline(cin, password);
	//Provjera ukoliko je uneseni username/password admina
	if(username == admin.administrator.username && password == admin.administrator.password) {
		return 234781; //Vraca se unikatna vrijednost samo za tu osobu
	} else {
		//U suprotnom, prolazi se kroz niz studenata koji su registrovani, te funkcija vraca broj indeksa tog studenta
		for (int i=0; i<brojStudenata; i++) {
			if(username == nizStudenata[i].student.username && password == nizStudenata[i].student.password) {
				return nizStudenata[i].brIndeksa;
			}
		}
	}
	cin.ignore();
}

int main () {
	//Dodaju se informacije za admina (koji je u nasem slucaju samo jedan)
	admin.administrator.ime = "Selman";					//Ime admina
	admin.administrator.prezime = "Patkovic";			//Prezime admina
	admin.administrator.username = "selman";			//Username: selman
	admin.administrator.password = "admin";				//Password: admin
	admin.specijalniID = 234781;						//Specijalni ID po kojem je admin prepoznatljiv
	admin.uloga = (korisnickaUloga)administrator;		//Default uloga admina
	
	//Home screen aplikacije
	cout << "\n\n\n\n\n";
	cout << crt << "\t\t\t::Dobrodosli u student management system::" << crt;
	cout << "\t\t\t\tPravio: Patkovic Selman\n\n";
	cout << "\t\t\t*Napomena*\n\n";
	cout << "\t\t\tZa potrebe testiranja administrator menija,\n";
	cout << "\t\t\tkao login podatke koristite:\n\n";
	cout << "\t\t\t\tusername: selman\n";
	cout << "\t\t\t\tpassword: admin\n\n\n\n";
	cout << "\t\tPritisnite ENTER da nastavite.............................";
	getchar();
	
	//Dodavanje 5 defaultnih pitanja
	DodajDefaultPitanja();
	//Dodavanje studenata iz baze podataka
	ucitajStudente();
	
	int odabir;	//Varijabla za odabir panela u glavnom meniju
	system("cls");
	
	//cout << "Broj studenata: " << brojStudenata << "\n\n";
	//Do while petlja koja radi sve dok se ne unese vrijednost 3, kojom se glavni meni zatvara
	do {
		int ID;	//Varijabla u koju se smjesta rezultat funkcije login()
		string username, password;
		system("cls");
		odabir = loginMeni();	//Poziva se glavni meni i njegova vrijednost se smjesta u varijablu "odabir"
		//Ukoliko je "odabir" 3 petlja se prekida
		if(!cin) {
			cin.clear();
    		cout << "\tGreska: Unesite broj od 1 do 3\n";
    		cin.ignore(100000, '\n');
		} else if (odabir == 3) {
				break;
			} else if (odabir == 1) {	//Ukoliko je "odabir" 1, provjerava se da li vrijednost ID pripada adminu ili studentima
				system("cls");
				ID = login(username, password);
				system("cls");
				if(ID == admin.specijalniID) {	//Ako pripada adminu, otvara se poseban "adminMeni()" za njega, u kojem ima 7 opcija
					system("cls");
					int izbor;
					do {
						izbor = adminMeni();
						if(izbor == 9) {
							break;
						}
						if (izbor == 1) {	//Opcija 1: Unos dodatnih pitanja
							unosPitanja();
							azurirajPodatke();
							cout << "\n="; Sleep(100); cout << "="; Sleep(100); cout << "="; Sleep(100); cout << "=";
							Sleep(100); cout << "="; Sleep(100); cout << " Novo pitanje uneseno\n";
							cout << "\n="; Sleep(100); cout << "="; Sleep(100); cout << "="; Sleep(100); cout << "=";
							Sleep(100); cout << "="; Sleep(100); cout << " Test resetovan za sve studente\n\n";
							Sleep(1500);
							for (int i=0; i<brojStudenata; i++) {
								nizStudenata[i].radioTest = false;
							}
						} else if (izbor == 2) {	//Opcija 2: Unos dodatnih studenata
							cin.ignore();
							unosPodataka();
						} else if (izbor == 3) {	//Opcija 3: Prikaz rezultata testova studenata
							if (brojStudenata>0) {
								Student &st = nizStudenata[prikaziStudente()];
								if(!st.radioTest) {
									cout << "\t\tStudent jos uvijek nije radio test!\n";
								} else {
									int t = dajBrojTacnih(st);
									cout << "\n\t\tBroj tacnih: " << t << endl;
									int p = 100*t/brojPitanja;
									cout << "\t\tUspjeh je " << p << " - (" << t << "/" << brojPitanja << ")\n\n";
								}		
							} else {
								cout << "Trenutno nema registrovanih studenata!\n";
								Sleep(2000);
								system("cls");
							}
						} else if (izbor == 4) {	//Opcija 4: Pretraga studenata na osnovu nekog od odabranih kriterija
							pretraga();
						} else if (izbor == 5) {	//Opcija 5: Resetovanje testa za odabranog studenta
							Student &st = nizStudenata[prikaziStudente()];
							st.radioTest = false;
							azurirajPodatke();
							cout << "\n="; Sleep(100); cout << "="; Sleep(100); cout << "="; Sleep(100); cout << "=";
							Sleep(100); cout << "="; Sleep(100); cout << " Test resetovan za studenta " << st.student.ime << " " << st.student.prezime << "\n\n";
						} else if (izbor == 6) {	//Opcija 6: Prikaz svih registrovanih studenata u sistemu
							prikaz();
						} else if (izbor == 7) {	//Opcija 7: Prikaz svih unesenih pitanja do tog momenta
							prikazPitanja();
						} else if (izbor == 8) {	//Opcija 8: Pohranjivanje svih unesenih podataka u datoteku "studenti.txt"
							evidencija();
						}
							system("PAUSE");
							system("cls");
					} while(1);				//Kraj admin menija
				} else {	//Ako ID nije pripadao adminu, onda se kroz for petlju ispod provjerava kojem studentu pripada broj indexa
					for(int i=0; i<brojStudenata; i++) {
						if(ID == nizStudenata[i].brIndeksa) {	//Ako petlja nadje datog studenta, pokrece se studentMeni() putem do while petlje ispod
							do {
								int izbor;
								izbor = studentMeni();
								if (izbor == 3) {	//Opcija 3: zatvara student meni
									break;
								} else if (izbor == 1) {	//Opcija 1: pokrece test ukoliko ga student nije radio
									if (nizStudenata[i].radioTest) {
										cout << "Test se moze raditi vise puta samo uz dozvolu admina!\n";
									} else {
										system("cls");
										cout << "\n\n\n\n";
										cout << crt << "\t\t\t\t\t::TEST::" << crt;
										for (int j=0; j<brojPitanja; j++) {	//Petlja prolazi kroz sva pitanja, ispisuje ih i nudi jedan od dva odgovora: 0/1
											cout << "\t\t" << j+1 << ". pitanje: ";
											prikaziPitanje(j);
											int x;
											do {
												cout << "\t\t\t0 - NETACNO, 1 - TACNO\n\t\t\tOdgovor: ";
												cin >> x;
											} while(x<0 || x>1);
											nizStudenata[i].odgovori[j] = (Odgovori)x; //Odgovori se spasavaju za datog studenta
										}
										nizStudenata[i].radioTest = true;	//Postavlja se vrijednost true, kako bi se znalo da je taj student radio test
										azurirajPodatke();	
									}			
								} else if (izbor == 2) {	//Opcija 2: Student moze pregledati rezultate testova svih studenata
									Student &st = nizStudenata[prikaziStudente()];
									if(!st.radioTest) {
									cout << "\t\tStudent jos uvijek nije radio test!\n";
									} else {
										int t = dajBrojTacnih(st);
										cout << "\n\t\tBroj tacnih: " << t << endl;
										int p = 100*t/brojPitanja;
										cout << "\t\tUspjeh je " << p << " - (" << t << "/" << brojPitanja << ")\n\n";
									}		
								}
									system("PAUSE");
									system("cls");	
							} while(1);	//Kraj student menija
							break;	//Prekid za for petlju nakon sto nadje odgovarajuceg studenta	
						} else {
							//Za pogresno unesenu sifru/username sistem nece nista uraditi, nego ce se ponovo ucitati login meni
						}
					}
				}
			} else if (odabir == 2) {	//Opcija 2 iz glavnog menija: Registracija novog studenta
				unosPodataka();
				cout << "\n\t\t="; Sleep(100); cout << "="; Sleep(100); cout << "="; Sleep(100); cout << "=";
				Sleep(100); cout << "="; Sleep(100); cout << " Korisnik uspjesno registrovan!\n";
				Sleep(1500);
				system("cls");
			} 
		} while(1);
		system("PAUSE");
		return 0;
}

