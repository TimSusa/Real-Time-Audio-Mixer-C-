#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>

// Struktur für Mixer-Kanal
struct MixerChannel
{
    int id;
    float volume;
    Mix_Chunk *audio; // Audiodatei
};

// Globale Variablen
std::vector<MixerChannel> mixerChannels;
SDL_Renderer *renderer = nullptr;

// Hilfsfunktion zum Rendern eines Rechtecks
void renderRect(SDL_Renderer *renderer, SDL_Rect rect, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void renderGUI()
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Background color
    SDL_RenderClear(renderer);

    int x = 50;
    for (auto &channel : mixerChannels)
    {
        // Fader background (inactive part)
        SDL_Rect faderBackground = {x, 100, 40, 200};
        renderRect(renderer, faderBackground, {60, 60, 60, 255}); // Dark gray background

        // Fader thumb (active part)
        SDL_Rect faderThumb = {x, 300 - static_cast<int>(channel.volume * 200), 40, static_cast<int>(channel.volume * 200)};
        renderRect(renderer, faderThumb, {100, 200, 255, 255}); // Light blue thumb

        // Outline around the fader thumb
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White outline
        SDL_RenderDrawRect(renderer, &faderThumb);

        x += 80; // Move to the next fader
    }

    SDL_RenderPresent(renderer);
}

// Event-Verarbeitung für die GUI
void handleEvents(bool &running, bool &isMouseDown)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
        {
            isMouseDown = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
        {
            isMouseDown = false;
        }
        else if (event.type == SDL_MOUSEMOTION && isMouseDown)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);

            int channelIndex = (x - 50) / 80; // Calculate fader region
            if (channelIndex >= 0 && channelIndex < mixerChannels.size() && y >= 100 && y <= 300)
            {
                float newVolume = (300 - y) / 200.0f;
                mixerChannels[channelIndex].volume = newVolume;

                // Update volume in the audio channel
                Mix_Volume(channelIndex, static_cast<int>(newVolume * MIX_MAX_VOLUME));
            }
        }
    }
}

// Initialisierung von SDL und SDL_mixer
bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL_Init Fehler: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer Fehler: " << Mix_GetError() << std::endl;
        return false;
    }

    SDL_Window *window = SDL_CreateWindow("Simple Mixer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 400, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "SDL_Window Fehler: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "SDL_Renderer Fehler: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Hauptfunktion
int main(int argc, char *argv[])
{
    if (!initSDL())
    {
        return 1;
    }

    // Test-Kanäle hinzufügen
    for (int i = 0; i < 3; ++i)
    {
        MixerChannel channel = {i, 0.5f, Mix_LoadWAV("1.wav")};
        if (!channel.audio)
        {
            std::cerr << "Fehler beim Laden der Audiodatei: " << Mix_GetError() << std::endl;
            return 1;
        }
        mixerChannels.push_back(channel);
        Mix_PlayChannel(i, channel.audio, -1); // Dauerschleife
    }

    bool running = true;
    bool isMouseDown = false;
    while (running)
    {
        handleEvents(running, isMouseDown);
        renderGUI();
    }

    // Aufräumen
    for (auto &channel : mixerChannels)
    {
        Mix_FreeChunk(channel.audio);
    }
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
