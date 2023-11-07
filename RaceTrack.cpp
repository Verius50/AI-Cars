#include "RaceTrack.h"
#include <iostream>
#include <fstream>

void RaceTrack::save()
{
    std::ofstream file("RaceTracks.txt", std::ios::app);
    if (file)
    {
        file << '\n' << name << " { "<<startPos.x<<' '<<startPos.y<<" }\n";
        for (size_t i = 0; i < lines.getVertexCount(); i++)
        {
            file << lines[i].position.x <<' ' << lines[i].position.y << ' ';
        }
        file << -1;
    }
}


int RaceTrack::read(int number)
{
    std::ifstream file("RaceTracks.txt");
    int racenumber = 0; char interact = '{';
    if (file)
        for (int i = 1; !file.eof(); i++)
        {
            this->clear();
            int x = 0, y = 0, x2 = 0, y2 = 0;
            file >> name >> interact >> startPos.x >> startPos.y >> interact;
            if (file.eof())
                return i - 1;
            for (int n = 2; x != -1; n += 2)
            {
                lines.resize(n);
                file >> x;
                if (x != -1)
                    file >> y >> x2 >> y2;
                else break;
                lines[n - 2].position = Vector2f(x, y);
                lines[n - 1].position = Vector2f(x2, y2);
            }
            if (number == i)
                return i;
            racenumber++;
        }
    file.close();
    return racenumber;
}

void RaceTrack::deleteRace(int number)
{
    std::ifstream file("RaceTracks.txt");
    std::ofstream outfile("temp.txt");
    int racenumber = 0, a= 0; std::string tempStr;
    char interact = '{';
    std::vector<int> allData;
    if (file)
        for (int i = 1; i < 50; i++)
        {
            allData.clear();
            file >> tempStr >> interact >> startPos.x >> startPos.y >> interact;
            if (file.eof()){
                file.close();
                outfile.close();
                std::cout<<std::remove("RaceTracks.txt");
                std::cout << std::rename("temp.txt", "RaceTracks.txt");
                return;
            }

            for (int n = 2; true; n += 2)
            {
                allData.resize(allData.size() + 1);
                file >> allData[allData.size() - 1];
                if (*(allData.end()-1) != -1)
                    {
                        allData.resize(allData.size() + 3);
                        file >> allData[allData.size() - 3] >> allData[allData.size() - 2] >> allData[allData.size() - 1];
                    }
                else break;
            }
            if (number != i){
                outfile << tempStr << " { " << startPos.x << ' ' << startPos.y << " }\n";
                for (size_t i = 0; i < allData.size(); i++)
                    outfile <<' ' << allData[i];
                outfile << '\n';
            }
            racenumber++;
        }

    file.close();
    outfile.close();
    std::cout << std::remove("RaceTracks.txt")<<'\n';
    std::cout<<std::rename("temp.txt", "RaceTracks2.txt");
}
