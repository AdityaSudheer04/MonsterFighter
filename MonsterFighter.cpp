#include<iostream>
#include<string>
#include<string_view>
#include<array>
#include"random.h"
#include<cstdlib>
#include<ctime>
#include<sstream>



class Potion
{
public:
    
    enum PotionType
    {
        health,
        strength,
        poison,
        max_type
    };

    enum PotionSize
    {
        small,
        medium,
        large,
        max_size
    };


private:
    PotionType m_type{};
    PotionSize m_size{};

public:
    Potion(PotionType type, PotionSize size)
        : m_type{ type },
        m_size{ size }
    {
    }

    PotionType getType() const { return m_type; }
    PotionSize getSize() const { return m_size; }

    
    static std::string_view getPotionTypeName(PotionType type)
    {
        static constexpr std::array names{
          "Health",
          "Strength",
          "Poison"
        };

        return names.at(static_cast<std::size_t>(type));
    }

    static std::string_view getPotionSizeName(PotionSize size)
    {
        static constexpr std::array names{
          "Small",
          "Medium",
          "Large"
        };

        return names.at(static_cast<std::size_t>(size));
    }

    std::string getName() const
    {
        
        std::stringstream result{};

        result << getPotionSizeName(getSize()) << " potion of " << getPotionTypeName(getType());

        
        return result.str();
    }

    static Potion getRandomPotion()
    {
        return {
          static_cast<PotionType>(Random::get(0, static_cast<int>(PotionType::max_type) - 1)),
          static_cast<PotionSize>(Random::get(0, static_cast<int>(PotionSize::max_size) - 1))
        };
    }
};


class Creature
{
protected:
    std::string m_name{};
    char m_symbol{};
    int m_Hp{};
    int m_Atk{};
    int m_gold{};

public:
    Creature(std::string_view name, char symbol, int Hp, int Atk, int gold)
        :m_name{name}
        ,m_symbol{symbol}
        ,m_Hp{Hp}
        ,m_Atk{Atk}
        ,m_gold{gold}
    {
    }

    std::string getName() const {return m_name;}
    char getSymbol() const {return m_symbol;}
    int getHp() const {return m_Hp;}
    int getAtk() const {return m_Atk;}
    int getGold() const {return m_gold;}

    void reduceHp(int negHp)
    {
        m_Hp -= negHp;
    }

    bool isDead() const {return m_Hp <= 0;}

    void addGold(int extragold)
    {
        m_gold += extragold;
    }
};

class Player : public Creature
{
private:
    int m_level{};
public:
    Player(std::string_view name)
        :Creature(name, '@', 10, 1, 0)
    {

    }

    void levelUp()
    {
        m_level += 1;
        m_Atk += 1;
    }
    
    int getLevel(){return m_level;}
    // int getAtk(){return m_Atk;}
    bool isWon(){return m_level == 20;}

    void drinkPotion(const Potion& potion)
    {
        switch (potion.getType())
        {
        case Potion::health:
            m_Hp += (potion.getSize() == Potion::large) ? 5: 2;
            break;
        case Potion::strength:
            ++m_Atk;
            break;
        case Potion::poison:
            reduceHp(1);
            break;
        case Potion::max_type:
            break;
        }
    }
};

class Monster : public Creature
{

public:
    enum class Type
    {
        dragon,
		orc,
		slime,
        max_types
    };
    

private:
    static const Creature& getDefaultCreature(Type type)
    {
        static const std::array<Creature, static_cast<std::size_t>(Type::max_types)> monsterData{
        {   { "dragon", 'D', 20, 4, 100 },
            { "orc", 'o', 4, 2, 25 },
            { "slime", 's', 1, 1, 10 } }
        };

    return monsterData.at(static_cast<std::size_t>(type));
    }

public:
    Monster(Type type)
        :Creature{getDefaultCreature(type)}
    {
    }

    static Monster getRandomMonster(){return static_cast<Type>(Random::get(0,(static_cast<int>(Type::max_types) - 1)));}
};


void onMonsterKilled(const Monster& m, Player& p)
{
    std::cout << "You killed the " << m.getName() << ".\n";
    p.levelUp();
    std::cout << "You are now level " << p.getLevel() << ".\n";
    std::cout << "You found " << m.getGold() << " gold.\n";
    p.addGold(m.getGold());

    constexpr int chance{30};
    if(Random::get(1,100) <= chance)
    {
        auto potion{Potion::getRandomPotion()};

        std::cout<<"You found a mythical potion. Do you want to drink it? [y/n]: ";
        char choice{};
        std::cin>>choice;

        if (choice == 'y' || choice == 'Y')
        {
            p.drinkPotion(potion);
            std::cout<<"Youo drank a "<<potion.getName()<<'\n';
        }
    }
}

void attackMonster(Monster& m, Player& p)
{
    if(p.isDead())
        return;
    
    std::cout<<"You hit the "<<m.getName()<<" for "<<p.getAtk()<<" damage.\n";

    m.reduceHp(p.getAtk());
    if(m.isDead())
    {
        onMonsterKilled(m,p);
    }
    
}

void attackPlayer(const Monster& m, Player& p)
{
    if(m.isDead())
        return;
    
    std::cout<<"The "<<m.getName()<<" hit you for "<<m.getAtk()<<" damage.\n";
        
    p.reduceHp(m.getAtk());

    std::cout<<"Your Level: "<<p.getLevel()<<"  HP : "<<p.getHp()<<"  Atk : "<<p.getAtk()<<'\n'; 
}

void fightMonster(Player& p)
{
    Monster m{ Monster::getRandomMonster() };
    std::cout<<"You have encountered a "<< m.getName() << " (" << m.getSymbol() <<").\n";
    std::cout<<" Dmg : "<<m.getAtk()<<"  HP : "<<m.getHp()<<'\n';
    while(!m.isDead() && !p.isDead())
    {
        std::cout<<"Your Level: "<<p.getLevel()<<"  HP : "<<p.getHp()<<"  Atk : "<<p.getAtk()<<'\n';
        std::cout<<"(R)un or (F)ight: ";
        char input;
        std::cin>>input;
         
        if(input == 'f' || input == 'F')
        {
            
            
            attackMonster(m,p);
            attackPlayer(m,p);
            
        }
        if(input == 'r' || input == 'R')
        {
            int chance{Random::get(0,1)};
            if(chance == 1)
            {    
                std::cout<<"You failed to flee.\n";
                attackPlayer(m,p);
                std::cout<<"\n\n";
                return;
                
            }
            else
            {
                std::cout<<"You successfully fled.\n\n";
                break;
            }
        }
        std::cout<<"\n\n";
    }
}




int main()
{
    
    
    std::cout << "Enter your name: ";
	std::string playerName;
	std::cin >> playerName;
    std::cout << "Welcome, " << playerName << ".\n";
	

    while (true)
    {
    
    Player p{ playerName };
	
	std::cout << "You have " << p.getHp() << " health and are carrying " << p.getGold() << " gold.\n";
    while(!p.isDead() && !p.isWon())
    {
        fightMonster(p);   
    }

    if (p.isDead())
    {
        std::cout << "You died at level " << p.getLevel() << " and with " << p.getGold() << " gold.\n";
        std::cout << "Too bad you can't take it with you!\n";
    }
    else
    {
        std::cout << "Congratulations!! You won the game with " << p.getGold() << " gold!\n";
    }

    std::cout<<"\nDo you want to play again or quit? [P/Q]: \n\n";
    char choice{};
    std::cin>>choice;
    if(choice == 'p' || choice == 'P')
    {
        continue;
    }
    else if(choice == 'q' || choice == 'Q')
    {
        break;
    }
    else
    {
        break;
    }

    }

    
	return 0;
}