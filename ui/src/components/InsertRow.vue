<script setup lang="ts">

import {
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogClose,
    Dialog,
    DialogTrigger
} from '@/components/ui/dialog'
import { Label } from '@/components/ui/label'
import {
    NumberField,
    NumberFieldContent,
    NumberFieldDecrement,
    NumberFieldIncrement,
    NumberFieldInput,
} from '@/components/ui/number-field'
import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'
import {
  Command,
  CommandEmpty,
  CommandGroup,
  CommandInput,
  CommandItem,
  CommandList,
} from '@/components/ui/command'

import {
  Popover,
  PopoverContent,
  PopoverTrigger,
} from '@/components/ui/popover'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import { Plus } from 'lucide-vue-next'
import { cn } from '@/lib/utils'

import { cbFetch } from '@/services/api-service';
import { ref } from 'vue';
import type { Cols } from '@/utils/types';

defineProps<{
    tableName: string
    cols: Cols[]
}>()

const frameworks = [
  { value: 'next.js', label: 'Next.js' },
  { value: 'sveltekit', label: 'SvelteKit' },
  { value: 'nuxt', label: 'Nuxt' },
  { value: 'remix', label: 'Remix' },
  { value: 'astro', label: 'Astro' },
]

const open = ref(false)
const value = ref('')

const error = ref('');

async function insertRow() {

}

function getInputType(type: string): string {
    if (type === 'BLOB') return 'file';
    return 'text'
}

const close = () => { error.value = ""; }
</script>

<template>
    <Dialog>

        <DialogTrigger>
            <TooltipProvider>
                <Tooltip>
                    <TooltipTrigger as-child>
                        <Button variant="outline">
                            <Plus />
                        </Button>
                    </TooltipTrigger>
                    <TooltipContent>
                        <p>Insert a row</p>
                    </TooltipContent>
                </Tooltip>
            </TooltipProvider>
        </DialogTrigger>
        <DialogContent>
            <DialogHeader>
                <DialogTitle>Insert a new row into {{ tableName }}</DialogTitle>
                <DialogDescription>

                </DialogDescription>
            </DialogHeader>

            <template v-for="col of cols">
                <NumberField v-if="['INTEGER', 'REAL'].includes(col.type)" :id="col.name"
                    :format-options="col.type === 'REAL' ? {
                        minimumFractionDigits: 2,
                    } : {}">
                    <Label :for="col.name">{{ col.name }}</Label>
                    <NumberFieldContent>
                        <NumberFieldDecrement />
                        <NumberFieldInput />
                        <NumberFieldIncrement />
                    </NumberFieldContent>
                </NumberField>

                <Popover v-else-if="col.type === 'ANY'" v-model:open="open">
                    <PopoverTrigger as-child>
                    <Button
                        variant="outline"
                        role="combobox"
                        :aria-expanded="open"
                        class="justify-between"
                    >
                        {{ value
                        ? frameworks.find((framework) => framework.value === value)?.label
                        : col.name }}
                        <CaretSortIcon class="ml-2 h-4 w-4 shrink-0 opacity-50" />
                    </Button>
                    </PopoverTrigger>
                    <PopoverContent class="p-0">
                    <Command>
                        <CommandInput class="h-9" placeholder="Search row..." />
                        <CommandEmpty>No row found.</CommandEmpty>
                        <CommandList>
                        <CommandGroup>
                            <CommandItem
                            v-for="framework in frameworks"
                            :key="framework.value"
                            :value="framework.value"
                            @select="(ev) => {
                                if (typeof ev.detail.value === 'string') {
                                value = ev.detail.value
                                }
                                open = false
                            }"
                            >
                            {{ framework.label }}
                            <CheckIcon
                                :class="cn(
                                'ml-auto h-4 w-4',
                                value === framework.value ? 'opacity-100' : 'opacity-0',
                                )"
                            />
                            </CommandItem>
                        </CommandGroup>
                        </CommandList>
                    </Command>
                    </PopoverContent>
                </Popover>

                <Input v-else :type="getInputType(col.type)" :disabled="!col.type" :placeholder="col.name" />
            </template>

            <pre v-if="error" class="text-destructive">{{ error }}</pre>

            <DialogFooter>
                <DialogClose as-child>
                    <Button @click="close" type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
                <Button @click="insertRow">Confirm</Button>
            </DialogFooter>
        </DialogContent>
    </Dialog>
</template>